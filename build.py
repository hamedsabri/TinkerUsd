#!/usr/bin/env python
from __future__ import print_function

from distutils.spawn import find_executable
from glob import glob

import argparse
import contextlib
import codecs
import datetime
import distutils
import multiprocessing
import os
import platform
import re
import shlex
import shutil
import subprocess
import sys
import distutils.util
import time
import zipfile
import tarfile
import json
import stat

############################################################
# Helpers for printing output
verbosity = 1

def Print(msg):
    if verbosity > 0:
        print(msg)

def PrintWarning(warning):
    if verbosity > 0:
        print("WARNING:", warning)

def PrintStatus(status):
    if verbosity >= 1:
        print("STATUS:", status)

def PrintInfo(info):
    if verbosity >= 2:
        print("INFO:", info)

def PrintCommandOutput(output):
    if verbosity >= 3:
        sys.stdout.write(output)

def PrintError(error):
    if verbosity >= 3 and sys.exc_info()[1] is not None:
        import traceback
        traceback.print_exc()
    print("ERROR:", error)

def Python3():
    return sys.version_info.major == 3
############################################################
def Windows():
    return platform.system() == "Windows"

def Linux():
    return platform.system() == "Linux"

def MacOS():
    return platform.system() == "Darwin"

def GetCommandOutput(command):
    """Executes the specified command and returns output or None."""
    try:
        return subprocess.check_output(
            shlex.split(command), stderr=subprocess.STDOUT).strip()
    except subprocess.CalledProcessError:
        pass
    return None

def GetGitHeadInfo(context):
    """Returns HEAD commit id and date."""
    try:
        with CurrentWorkingDirectory(context.srcDir):
            commitSha = subprocess.check_output('git rev-parse HEAD', shell = True).decode()
            commitDate = subprocess.check_output('git show -s HEAD --format="%ad"', shell = True).decode()
            return commitSha, commitDate
    except Exception as exp:
        PrintError("Failed to run git commands : {exp}".format(exp=exp))
        sys.exit(1)

def GetXcodeDeveloperDirectory():
    """Returns the active developer directory as reported by 'xcode-select -p'.
    Returns None if none is set."""
    if not MacOS():
        return None

    return GetCommandOutput("xcode-select -p")

def GetVisualStudioCompilerAndVersion():
    """Returns a tuple containing the path to the Visual Studio compiler
    and a tuple for its version, e.g. (14, 0). If the compiler is not found
    or version number cannot be determined, returns None."""
    if not Windows():
        return None

    msvcCompiler = find_executable('cl')
    if msvcCompiler:
        # VisualStudioVersion environment variable should be set by the
        # Visual Studio Command Prompt.
        match = re.search(
            r"(\d+)\.(\d+)",
            os.environ.get("VisualStudioVersion", ""))
        if match:
            return (msvcCompiler, tuple(int(v) for v in match.groups()))
    return None

def IsVisualStudioVersionOrGreater(desiredVersion):
    if not Windows():
        return False

    msvcCompilerAndVersion = GetVisualStudioCompilerAndVersion()
    if msvcCompilerAndVersion:
        _, version = msvcCompilerAndVersion
        return version >= desiredVersion
    return False

def IsVisualStudio2022OrGreater():
    VISUAL_STUDIO_2022_VERSION = (17, 0)
    return IsVisualStudioVersionOrGreater(VISUAL_STUDIO_2022_VERSION)

def IsVisualStudio2019OrGreater():
    VISUAL_STUDIO_2019_VERSION = (16, 0)
    return IsVisualStudioVersionOrGreater(VISUAL_STUDIO_2019_VERSION)

def IsVisualStudio2017OrGreater():
    VISUAL_STUDIO_2017_VERSION = (15, 0)
    return IsVisualStudioVersionOrGreater(VISUAL_STUDIO_2017_VERSION)

def GetCPUCount():
    try:
        return multiprocessing.cpu_count()
    except NotImplementedError:
        return 1

def Run(context, cmd):
    """Run the specified command in a subprocess."""
    PrintInfo('Running "{cmd}"'.format(cmd=cmd))

    with codecs.open(context.logFileLocation, "a", "utf-8") as logfile:
        logfile.write("#####################################################################################" + "\n")
        logfile.write("log date: " + datetime.datetime.now().strftime("%Y-%m-%d %H:%M") + "\n")
        commitID,commitData = GetGitHeadInfo(context)
        logfile.write("commit sha: " + commitID)
        logfile.write("commit date: " + commitData)
        logfile.write("#####################################################################################" + "\n")
        logfile.write("\n")
        logfile.write(cmd)
        logfile.write("\n")

        # Let exceptions escape from subprocess calls -- higher level
        # code will handle them.
        if context.redirectOutstreamFile:
            p = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT)
            encoding = sys.stdout.encoding or "UTF-8"
            while True:
                l = p.stdout.readline().decode(encoding)
                if l != "":
                    # Avoid "UnicodeEncodeError: 'ascii' codec can't encode 
                    # character" errors by serializing utf8 byte strings.
                    logfile.write(l)
                    PrintCommandOutput(l)
                elif p.poll() is not None:
                    break
        else:
            p = subprocess.Popen(shlex.split(cmd))
            p.wait()

    if p.returncode != 0:
        # If verbosity >= 3, we'll have already been printing out command output
        # so no reason to print the log file again.
        if verbosity < 3:
            with open(context.logFileLocation, "r") as logfile:
                Print(logfile.read())
        raise RuntimeError("Failed to run '{cmd}'\nSee {log} for more details."
                           .format(cmd=cmd, log=os.path.abspath(context.logFileLocation)))

def BuildVariant(context): 
    if context.buildDebug:
        return "Debug"
    elif context.buildRelease:
        return "Release"
    elif context.buildRelWithDebug:
        return "RelWithDebInfo"
    return "RelWithDebInfo"

def FormatMultiProcs(numJobs, generator):
    tag = "-j"
    if generator:
        if "Visual Studio" in generator:
            tag = "/M:"
        elif "Xcode" in generator:
            tag = "-j "

    return "{tag}{procs}".format(tag=tag, procs=numJobs)

def onerror(func, path, exc_info):
    """
    If the error is due to an access error (read only file)
    add write permission and then retries.
    If the error is for another reason it re-raises the error.
    """
    import stat
    if not os.access(path, os.W_OK):
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise

def StartBuild():
    global start_time
    start_time = time.time()

def StopBuild():
    end_time = time.time()
    elapsed_seconds = end_time - start_time
    hours, remainder = divmod(elapsed_seconds, 3600)
    minutes, seconds = divmod(remainder, 60)
    print("Elapsed time: {:02}:{:02}:{:02}".format(int(hours), int(minutes), int(seconds)))

def ParseJson(file_path):
    """
    Read a JSON file and return the parsed data.

    Args:
        file_path (str): The path to the JSON file.

    Returns:
        dict: The parsed JSON data.
    """
    with open(file_path) as f:
        data = json.load(f)
    return data
############################################################
# contextmanager
@contextlib.contextmanager
def CurrentWorkingDirectory(dir):
    """Context manager that sets the current working directory to the given
    directory and resets it to the original directory when closed."""
    curdir = os.getcwd()
    os.chdir(dir)
    try:
        yield
    finally:
        os.chdir(curdir)

############################################################
# CMAKE
def RunCMake(context, extraArgs=None, stages=None):
    """Invoke CMake to configure, build, and install a library whose 
    source code is located in the current working directory."""

    srcDir = os.getcwd()
    instDir = context.instDir
    buildDir = context.buildDir

    if 'clean' in stages and os.path.isdir(buildDir):
        shutil.rmtree(buildDir, onerror=onerror)

    if 'clean' in stages and os.path.isdir(instDir):
        os.chmod(instDir, stat.S_IWRITE)
        shutil.rmtree(instDir)

    if not os.path.isdir(buildDir):
        os.makedirs(buildDir)

    generator = context.cmakeGenerator

    # On Windows, we need to explicitly specify the generator to ensure we're
    # building a 64-bit project. (Surely there is a better way to do this?)
    # TODO: figure out exactly what "vcvarsall.bat x64" sets to force x64
    if generator is None and Windows():
        generator = "Visual Studio 17 2022"

    if generator is not None:
        generator = '-G "{gen}"'.format(gen=generator)

    if generator and 'Visual Studio' in generator:
        generator = generator + " -A x64"

    # get build variant 
    variant= BuildVariant(context)

    with CurrentWorkingDirectory(buildDir):
        # recreate build_log.txt everytime the script runs
        if os.path.isfile(context.logFileLocation):
            os.remove(context.logFileLocation)

        if 'configure' in stages:
            Run(context,
                'cmake '
                '-DCMAKE_INSTALL_PREFIX="{instDir}" '
                '-DCMAKE_BUILD_TYPE={variant} '
                '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON '
                '{generator} '
                '{extraArgs} '
                '"{srcDir}"'
                .format(instDir=instDir,
                        variant=variant,
                        srcDir=srcDir,
                        generator=(generator or ""),
                        extraArgs=(" ".join(extraArgs) if extraArgs else "")))
 
        installArg = ""
        if 'install' in stages:
            installArg = "--target install"

        if 'build' in stages or 'install' in stages:
            Run(context, "cmake --build . --config {variant} {installArg} -- {multiproc}"
                .format(variant=variant,
                        installArg=installArg,
                        multiproc=FormatMultiProcs(context.numJobs, generator)))


def BuildAndInstall(context, buildArgs, stages):
    with CurrentWorkingDirectory(context.srcDir):
        extraArgs = []
        stagesArgs = []
        if context.pxrUsdLocation:
            extraArgs.append('-DPXR_USD_LOCATION="{pxrUsdLocation}"'
                             .format(pxrUsdLocation=context.pxrUsdLocation))
            extraArgs.append('-DCMAKE_WANT_MATERIALX_BUILD=ON')
            extraArgs.append('-DCMAKE_PREFIX_PATH="{materialxLocation}"'
                             .format(materialxLocation=context.pxrUsdLocation))
        if context.qtLocation:
            extraArgs.append('-DQT_LOCATION="{qtLocation}"'
                             .format(qtLocation=context.qtLocation))

        extraArgs += buildArgs
        stagesArgs += stages

        RunCMake(context, extraArgs, stagesArgs)

        # Ensure directory structure is created and is writable.
        for dir in [context.buildDir, context.instDir]:
            try:
                if os.path.isdir(dir):
                    testFile = os.path.join(dir, "canwrite")
                    open(testFile, "w").close()
                    os.remove(testFile)
                else:
                    os.makedirs(dir)
            except Exception as e:
                PrintError("Could not write to directory {dir}. Change permissions "
                           "or choose a different location to install to."
                           .format(dir=dir))
                sys.exit(1)
        Print("""Success build and install !!!!""")


############################################################
# ArgumentParser
parser = argparse.ArgumentParser(
    formatter_class=argparse.RawDescriptionHelpFormatter)

parser.add_argument("-v", "--verbosity", type=int, default=verbosity,
                    help=("How much output to print while building: 0 = no "
                          "output; 1 = warnings + status; 2 = info; 3 = "
                          "command output and tracebacks (default: "
                          "%(default)s)"))

parser.add_argument("--generator", type=str,
                    help=("CMake generator to use when building libraries with cmake"))

parser.add_argument("--build-location", type=str,
    help=("Set Build directory"))

parser.add_argument("--install-location", type=str,
    help=("Set Install directory"))

parser.add_argument("--pxrusd-location", type=str,
                    help="Directory where Pixar USD is installed.")

parser.add_argument("--qt-location", type=str,
                    help="Directory where QT is installed.")

parser.add_argument("--build-args", type=str, nargs="*", default=[],
                   help=("Comma-separated list of arguments passed into CMake when building libraries"))

varGroup = parser.add_mutually_exclusive_group()
varGroup.add_argument("--build-debug", dest="build_debug", action="store_true",
                    help="Build in Debug mode (default: %(default)s)")

varGroup.add_argument("--build-release", dest="build_release", action="store_true",
                    help="Build in Release mode (default: %(default)s)")

varGroup.add_argument("--build-relwithdebug", dest="build_relwithdebug", action="store_true", default=True,
                    help="Build in RelWithDebInfo mode (default: %(default)s)")

parser.add_argument("--stages", type=str, nargs="*", default=['clean','configure','build','install'],
                   help=("Comma-separated list of stages to execute. Possible stages: clean, configure, build, install, test, package."))

parser.add_argument("-j", "--jobs", type=int, default=GetCPUCount(),
                    help=("Number of build jobs to run in parallel. "
                          "(default: # of processors [{0}])"
                          .format(GetCPUCount())))

parser.add_argument("--redirect-outstream-file", type=distutils.util.strtobool, dest="redirect_outstream_file", default=True,
                    help="Redirect output stream to a file. Set this flag to false to redirect output stream to console instead.")

args = parser.parse_args()
verbosity = args.verbosity

############################################################
# InstallContext
class InstallContext:
    def __init__(self, args):

        # Assume the project's top level cmake is in the current source directory
        self.srcDir = os.path.normpath(
            os.path.join(os.path.abspath(os.path.dirname(__file__))))

        # Build type
        # Must be done early, so we can call BuildVariant(self)
        self.buildDebug = args.build_debug
        self.buildRelease = args.build_release
        self.buildRelWithDebug = args.build_relwithdebug

        buildSettingFilepath = os.path.join(self.srcDir, "build_settings.json")
        if os.path.exists(buildSettingFilepath):
            self.jsonData = ParseJson(buildSettingFilepath)
        else:
            PrintError("build_settings.json is missing!")

        buildArgs_json = ["{}={}".format(key, os.path.expandvars(value).replace('\\', '/')) for key, value in self.jsonData.get("build_args", {}).items()]
        cmakeGenerator_json = self.jsonData.get("cmake_generator")
        qtLocation_json = os.path.expandvars(self.jsonData.get("qt_location")).replace('\\', '/')
        pxrUsdLocation_json = os.path.expandvars(self.jsonData.get("pxrusd_location")).replace('\\', '/')
        buildDir_json = os.path.expandvars(self.jsonData.get("build_location")).replace('\\', '/')
        instDir_json = os.path.expandvars(self.jsonData.get("install_location")).replace('\\', '/')

        buildArgs_cl = list()
        for argList in args.build_args:
            for arg in argList.split(","):
                buildArgs_cl.append(arg)

        cmakeGenerator_cl = args.generator
        qtLocation_cl = (os.path.abspath(args.qt_location) if args.qt_location else None)
        pxrUsdLocation_cl = (os.path.abspath(args.pxrusd_location) if args.pxrusd_location else None)
        buildDir_cl = (os.path.abspath(args.build_location) if args.build_location else None)
        instDir_cl = (os.path.abspath(args.install_location) if args.install_location else None)

        # if the command line argument is not provided, we should fall back to the value from the JSON file.
        self.buildArgs = buildArgs_cl if buildArgs_cl else buildArgs_json
        self.cmakeGenerator = cmakeGenerator_cl if cmakeGenerator_cl else cmakeGenerator_json
        self.qtLocation = qtLocation_cl if qtLocation_cl else qtLocation_json
        self.pxrUsdLocation = pxrUsdLocation_cl if pxrUsdLocation_cl else pxrUsdLocation_json

        self.buildDir = buildDir_cl if buildDir_cl else buildDir_json
        self.instDir = instDir_cl if instDir_cl else instDir_json

        # Number of jobs
        self.numJobs = args.jobs
        if self.numJobs <= 0:
            raise ValueError("Number of jobs must be greater than 0")

        # Log File Name
        logFileName="build_log.txt"
        self.logFileLocation=os.path.join(self.buildDir, logFileName)

        # Stages arguments
        self.stagesArgs = list()
        for argList in args.stages:
            for arg in argList.split(","):
                self.stagesArgs.append(arg)

        # Redirect output stream to file
        self.redirectOutstreamFile = args.redirect_outstream_file
try:
    context = InstallContext(args)
except Exception as e:
    PrintError(str(e))
    sys.exit(1)

if __name__ == "__main__":
    # Summarize
    summaryMsg = """
    Building with settings:
      Source directory          {srcDir}
      Build directory           {buildDir}
      Install directory         {instDir}
      Variant                   {buildVariant}
      CMake generator           {cmakeGenerator}"""

    if context.redirectOutstreamFile:
      summaryMsg += """
      Build Log                 {logFileLocation}"""

    if context.buildArgs:
      summaryMsg += """
      Build arguments           {buildArgs}"""

    if context.stagesArgs:
      summaryMsg += """
      Stages arguments          {stagesArgs}"""


    summaryMsg = summaryMsg.format(
        srcDir=context.srcDir,
        buildDir=context.buildDir,
        instDir=context.instDir,
        logFileLocation=context.logFileLocation,
        buildArgs=context.buildArgs,
        stagesArgs=context.stagesArgs,
        buildVariant=BuildVariant(context),
        cmakeGenerator=("Default" if not context.cmakeGenerator
                        else context.cmakeGenerator)
    )

    Print(summaryMsg)

    # BuildAndInstall
    if any(stage in ['clean', 'configure', 'build', 'install'] for stage in context.stagesArgs):
        StartBuild()
        BuildAndInstall(context, context.buildArgs, context.stagesArgs)
        StopBuild()
