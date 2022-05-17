using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Diagnostics;
using System.Reflection;

class Program
{
    public class CommandRunner
    {
        public static string BuildFileExtension = ".TMNE_BUILD";

        public enum LogLevel
        {
            Warning,
            Error,
            Info
        };

        public enum BuildType
        {
            Debug,
            Release
        };

        public enum BuildPlatform
        {
            Windows,
            MacOS,
            Linux
        };

        public struct BuildSettings
        {
            public enum UnitType
            {
                Invalid,
                Project,
                Module
            };

            public string Title;
            public string OutputPath;
            public UnitType Type;
            public Dictionary<string, List<string>> CompileFlags;
            public List<string> SourceFiles;
            public List<string> IncludePaths;
            public List<string> LibFiles;
        }

        public void LogMessage(LogLevel level, string text)
        {
            string appName = "TMNE Build - ";
            if (level == LogLevel.Error)
            {
                Console.Error.WriteLine(appName + "Error: " + text);
            }
            else if (level == LogLevel.Warning)
            {
                Console.WriteLine(appName + "Warning: " + text);
            }
            else
            {
                Console.WriteLine(appName + "Info: " + text);
            }
        }

        public BuildSettings CreateSettings(string jsonString, BuildPlatform buildPlatform, BuildType buildType, string rootPath)
        {
            BuildSettings settings = new BuildSettings();

            JsonNode rootNode = JsonNode.Parse(jsonString);
            if (rootNode == null)
            {
                return settings;
            }

            // Metadata
            settings.Title = rootNode["Title"].GetValue<string>();
            settings.OutputPath = Path.GetFullPath(rootNode["OutputPath"].GetValue<string>(), rootPath);
            settings.Type = BuildSettings.UnitType.Invalid;
            if (String.Equals(rootNode["Type"].GetValue<string>(), "Project"))
            {
                settings.Type = BuildSettings.UnitType.Project;
            }
            else if (String.Equals(rootNode["Type"].GetValue<string>(), "Module"))
            {
                settings.Type = BuildSettings.UnitType.Module;
            }

            // Flags
            JsonObject compilerFlagGroups = rootNode["CompilerFlags"][buildPlatform.ToString()].AsObject();
            if (compilerFlagGroups == null)
            {
                return settings;
            }

            settings.CompileFlags = new Dictionary<string, List<string>>();
            foreach (var flagGroup in compilerFlagGroups)
            {
                if (flagGroup.Key == "Debug" && buildType == BuildType.Release)
                {
                    continue;
                }
                if (flagGroup.Key == "Release" && buildType == BuildType.Debug)
                {
                    continue;
                }
                
                JsonArray flags = flagGroup.Value.AsArray();                   
                string[] flagList = new string[flags.Count];
                for (int i = 0; i < flags.Count; ++i)
                {
                    flagList[i] = flags[i].ToString();        
                }

                settings.CompileFlags.Add(flagGroup.Key.ToString(), flagList.ToList());
            }

            // Source files
            JsonArray directoriesToScan = rootNode["DirectoriesToScan"].AsArray();
            JsonArray requiredModules = rootNode["Modules"].AsArray();
            List<string> moduleList = new List<string>();
            foreach (var module in requiredModules)
            {
                moduleList.Add(module.ToString());
            }

            settings.SourceFiles = new List<string>();
            settings.LibFiles = new List<string>();
            settings.IncludePaths = new List<string>();

            // Module scanning
            foreach (var path in directoriesToScan)
            {
                string fullPath = Path.GetFullPath(path.ToString(), rootPath);
                string[] foundModules = Directory.GetFiles(fullPath, "*" + BuildFileExtension, SearchOption.AllDirectories);
                foreach (string module in foundModules)
                {
                    // Make sure it's a actually a module
                    string moduleJsonString = File.ReadAllText(module);
                    if (string.IsNullOrEmpty(jsonString))
                    {
                        continue;
                    }

                    JsonNode moduleNode = JsonNode.Parse(moduleJsonString);
                    if (String.Equals(moduleNode["Type"].GetValue<string>(), "Project"))
                    {
                        continue;
                    }

                    if (!moduleList.Exists(title => String.Equals(title, moduleNode["Title"].GetValue<string>())))
                    {
                        continue;
                    }

                    // TODO: Compile each module as a static library to link 
                    string moduleRootPath = new FileInfo(Path.GetFullPath(module)).DirectoryName;
                    string[] cppSourceFiles = Directory.GetFiles(moduleRootPath, "*.cpp", SearchOption.AllDirectories);
                    settings.SourceFiles.AddRange(cppSourceFiles);

                    if (buildPlatform == BuildPlatform.MacOS) {
                        string[] objCSourceFiles = Directory.GetFiles(moduleRootPath, "*.mm", SearchOption.AllDirectories);
                        settings.SourceFiles.AddRange(objCSourceFiles);
                    }

                    settings.IncludePaths.Add(moduleRootPath);

                    // 3rdParty dependencies
                    JsonArray sourceFileDependencies = moduleNode["Dependencies"]["Files"].AsArray();
                    foreach (var file in sourceFileDependencies)
                    {
                        if (string.IsNullOrWhiteSpace(file.ToString())) {
                            continue;
                        }

                        string sourceFilePath = Path.GetFullPath(file.ToString(), rootPath);
                        settings.SourceFiles.Add(sourceFilePath);
                    }

                    JsonArray includeFileDependencies = moduleNode["Dependencies"]["Includes"].AsArray();
                    foreach (var include in includeFileDependencies)
                    {
                        if (string.IsNullOrWhiteSpace(include.ToString())) {
                            continue;
                        }

                        string includeFilePath = Path.GetFullPath(include.ToString(), rootPath);
                        settings.IncludePaths.Add(includeFilePath);
                    }

                    JsonObject moduleCompilerFlagGroups = moduleNode["CompilerFlags"][buildPlatform.ToString()].AsObject();
                    foreach (var flagGroup in moduleCompilerFlagGroups)
                    {
                        JsonArray flags = flagGroup.Value.AsArray();                   
                        string[] flagList = new string[flags.Count];
                        for (int i = 0; i < flags.Count; ++i)
                        {
                            flagList[i] = flags[i].ToString();

                            // Add Superluminal header and static library
                            // TODO: Support DLL loading
                            if (String.Equals(flagList[i], "-DCORE_PROFILER_SUPERLUMINAL=1", StringComparison.OrdinalIgnoreCase)) {
                                string superluminalPath = Environment.GetEnvironmentVariable("SUPERLUMINAL_API_DIR");
                                string superluminalInclude = superluminalPath + Path.DirectorySeparatorChar + "include"; 
                                settings.IncludePaths.Add(superluminalInclude);

                                string superluminalLibName = "";
                                if (buildType == BuildType.Debug) {
                                    if (settings.CompileFlags.ContainsKey("Debug")) {
                                        if (settings.CompileFlags["Debug"].Contains("-MTd")) {
                                            superluminalLibName = "PerformanceAPI_MTd.lib";
                                        }
                                        else if (settings.CompileFlags["Debug"].Contains("-MDd")) {
                                            superluminalLibName = "PerformanceAPI_MDd.lib";
                                        }
                                    }
                                }
                                else if (buildType == BuildType.Release) {
                                    if (settings.CompileFlags.ContainsKey("Release")) {
                                        if (settings.CompileFlags["Release"].Contains("-MT")) {
                                            superluminalLibName = "PerformanceAPI_MT.lib";
                                        }
                                        else if (settings.CompileFlags["Release"].Contains("-MD")) {
                                            superluminalLibName = "PerformanceAPI_MD.lib";
                                        }
                                    }
                                }

                                if (!String.IsNullOrEmpty(superluminalLibName)) {
                                    string superluminalLib = superluminalPath + Path.DirectorySeparatorChar + "lib" + Path.DirectorySeparatorChar + "x64" + Path.DirectorySeparatorChar + superluminalLibName; 
                                    settings.LibFiles.Add(superluminalLib);
                                }
                            } 
                        }

                        if (settings.CompileFlags.ContainsKey(flagGroup.Key.ToString())) {

                            settings.CompileFlags[flagGroup.Key.ToString()].AddRange(flagList);
                        }
                    }
                }
            }

            // Scan for this project
            string[] projectSourceFiles = Directory.GetFiles(rootPath, "*.cpp", SearchOption.AllDirectories);
            settings.SourceFiles.AddRange(projectSourceFiles);
            if (!settings.IncludePaths.Exists(x => String.Equals(x, rootPath))) {
                settings.IncludePaths.Add(rootPath);
            }

            return settings;
        }

        public bool ValidateSettings(BuildSettings settings)
        {
            if (string.IsNullOrEmpty(settings.Title))
            {
                LogMessage(LogLevel.Error, "Invalid field: Title");
                return false;
            }

            if (settings.Type == BuildSettings.UnitType.Invalid)
            {
                LogMessage(LogLevel.Error, "Invalid field: Type");
                return false;
            }

            return true;
        }

        public void WindowsBuild(string projectPath, BuildPlatform buildPlatform, BuildType buildType)
        {
            if (string.IsNullOrEmpty(projectPath))
            {
                LogMessage(LogLevel.Error, "Invalid " + BuildFileExtension + " file!");
                return;
            }

            FileInfo projectFile = new FileInfo(Path.GetFullPath(projectPath));
            if (!projectFile.Exists || projectFile.Extension != BuildFileExtension)
            {
                LogMessage(LogLevel.Error, "Invalid " + BuildFileExtension + " file!");
                return;
            }

            string jsonString = File.ReadAllText(projectFile.FullName);
            if (string.IsNullOrEmpty(jsonString))
            {
                LogMessage(LogLevel.Error, "Invalid " + BuildFileExtension + " file!");
                return;
            }

            // Load the settings file and build the command list
            BuildSettings buildSettings = CreateSettings(jsonString, buildPlatform, buildType, projectFile.DirectoryName);
            if (!ValidateSettings(buildSettings))
            {
                LogMessage(LogLevel.Error, "Invalid " + BuildFileExtension + " file!");
                return;
            }

            // Create a compiler process and run the commands
            string executablePath = Assembly.GetExecutingAssembly().Location;
            executablePath = Path.GetDirectoryName(executablePath);

            string argumentList = "";
            string compiler = "cl";
            string linkerList = "/link ";
            string formattedCommandList = "";

            foreach (var flags in buildSettings.CompileFlags)
            {
                if (String.Equals(flags.Key, "Linker", StringComparison.OrdinalIgnoreCase))
                {
                    foreach (var flag in flags.Value)
                    {
                        linkerList += flag + " ";
                        formattedCommandList += flag + " \n";
                    }
                }
                else
                {
                    foreach (var flag in flags.Value)
                    {
                        argumentList += flag + " ";
                        formattedCommandList += flag + " \n";
                    }
                }
            }

            string outputList = "";
            if (buildSettings.Type == BuildSettings.UnitType.Project)
            {
                string baseOutputPath = buildSettings.OutputPath + Path.DirectorySeparatorChar + buildType.ToString() + Path.DirectorySeparatorChar;
                Directory.CreateDirectory(baseOutputPath);
                outputList += "-Fe" + baseOutputPath + buildSettings.Title + ".exe";
                outputList += " -Fo" + baseOutputPath;
                outputList += " -Fd" + baseOutputPath + " ";

                formattedCommandList += "-Fe" + baseOutputPath + buildSettings.Title + ".exe" + " \n";
                formattedCommandList += "-Fo" + baseOutputPath + " \n";
                formattedCommandList += "-Fd" + baseOutputPath  + " \n";
            }

            string fileList = "";
            foreach (string file in buildSettings.SourceFiles)
            {
                fileList += "\"" + file + "\" ";
                formattedCommandList += "\"" + file + "\" " + " \n";
            }

            foreach (string file in buildSettings.LibFiles)
            {
                fileList += "\"" + file + "\" ";
                formattedCommandList += "\"" + file + "\" " + " \n";
            }

            string includeList = "";
            foreach (string path in buildSettings.IncludePaths)
            {
                string includePath = "-I\"" + path + "\" ";
                includeList += includePath;
                formattedCommandList += includePath + " \n";
            }

            string commandListTotal = compiler + " " + argumentList + outputList + includeList + fileList + linkerList;

            LogMessage(LogLevel.Info, buildSettings.Type.ToString() + " " + buildSettings.Title + " (" + buildPlatform.ToString() + " - " + buildType.ToString() + ")");
            LogMessage(LogLevel.Info, argumentList + linkerList);

            // TODO: Find the .git folder to base as the root?
            string repoRoot = Path.GetFullPath(executablePath + "/../../../");
            string compileCommandsFile = repoRoot + "compile_flags.txt";
            File.Delete(compileCommandsFile);
            var fileHandle = File.Create(compileCommandsFile);
            fileHandle.Write(Encoding.UTF8.GetBytes(formattedCommandList), 0, Encoding.UTF8.GetByteCount(formattedCommandList));
            fileHandle.Close();

            Process compilerProcess = new Process();
            compilerProcess.StartInfo.FileName = "cmd.exe";
            compilerProcess.StartInfo.Arguments = "/c call " + commandListTotal;
            compilerProcess.StartInfo.UseShellExecute = false;
            compilerProcess.StartInfo.RedirectStandardOutput = true;
            compilerProcess.StartInfo.RedirectStandardError = true;
            compilerProcess.StartInfo.CreateNoWindow = true;
            compilerProcess.Start();

            while (!compilerProcess.StandardOutput.EndOfStream)
            {
                string currentLine = compilerProcess.StandardOutput.ReadLine();
                Console.WriteLine(currentLine);
            }

            compilerProcess.WaitForExit();
        }

        public void MacOSBuild(string projectPath, BuildPlatform buildPlatform, BuildType buildType)
        {
            if (string.IsNullOrEmpty(projectPath))
            {
                LogMessage(LogLevel.Error, "Invalid " + BuildFileExtension + " file!");
                return;
            }

            FileInfo projectFile = new FileInfo(Path.GetFullPath(projectPath));
            if (!projectFile.Exists || projectFile.Extension != BuildFileExtension)
            {
                LogMessage(LogLevel.Error, "Invalid " + BuildFileExtension + " file!");
                return;
            }

            string jsonString = File.ReadAllText(projectFile.FullName);
            if (string.IsNullOrEmpty(jsonString))
            {
                LogMessage(LogLevel.Error, "Invalid " + BuildFileExtension + " file!");
                return;
            }

            // Load the settings file and build the command list
            BuildSettings buildSettings = CreateSettings(jsonString, buildPlatform, buildType, projectFile.DirectoryName);
            if (!ValidateSettings(buildSettings))
            {
                LogMessage(LogLevel.Error, "Invalid " + BuildFileExtension + " file!");
                return;
            }

            // Create a compiler process and run the commands
            string executablePath = Assembly.GetExecutingAssembly().Location;
            executablePath = Path.GetDirectoryName(executablePath);

            string argumentList = "";
            string compiler = "clang";
            string linkerList = "";

            // TODO: Find the .git folder to base as the root?
            string repoRoot = Path.GetFullPath(executablePath + "/../../../");
            string compileCommandsFile = repoRoot + "compile_flags.txt";
            File.Delete(compileCommandsFile);
            File.Open(compileCommandsFile, FileMode.OpenOrCreate, FileAccess.Write, FileShare.ReadWrite);

            foreach (var flags in buildSettings.CompileFlags)
            {
                if (String.Equals(flags.Key, "Linker", StringComparison.OrdinalIgnoreCase))
                {
                    foreach (var flag in flags.Value)
                    {
                        linkerList += " " + flag;
                        File.AppendAllText(compileCommandsFile, flag + Environment.NewLine);
                    }
                }
                else
                {
                    foreach (var flag in flags.Value)
                    {
                        argumentList += " " + flag;
                        File.AppendAllText(compileCommandsFile, flag + Environment.NewLine);
                    }
                }
            }

            string outputList = "";
            if (buildSettings.Type == BuildSettings.UnitType.Project)
            {
                string baseOutputPath = buildSettings.OutputPath + Path.DirectorySeparatorChar + buildType.ToString() + Path.DirectorySeparatorChar;
                Directory.CreateDirectory(baseOutputPath);
                outputList += " -o" + baseOutputPath + buildSettings.Title + " ";
                File.AppendAllText(compileCommandsFile, "-o" + baseOutputPath + buildSettings.Title + Environment.NewLine);
            }

            string fileList = "";
            foreach (string file in buildSettings.SourceFiles)
            {
                fileList += file + " ";
            }

            string includeList = "";
            foreach (string path in buildSettings.IncludePaths)
            {
                string includePath = "-I" + path + " ";
                includeList += includePath;
                File.AppendAllText(compileCommandsFile, includePath + Environment.NewLine);
            }

            // TODO: Can't get the terminal to work on MacOS - just print the commandlist for the time being
            string commandListTotal = compiler + argumentList + outputList + includeList + fileList + linkerList;
            Console.Write(commandListTotal);

            // LogMessage(LogLevel.Info, buildSettings.Type.ToString() + " " + buildSettings.Title + " (" + buildPlatform.ToString() + " - " + buildType.ToString() + ")");
            // LogMessage(LogLevel.Info, compiler + argumentList + linkerList);

            // Process compilerProcess = new Process();
            // compilerProcess.StartInfo.FileName = executablePath + "/../../../BuildProject.sh";
            // compilerProcess.StartInfo.Arguments = argumentList + outputList + includeList + fileList + linkerList;
            // compilerProcess.StartInfo.UseShellExecute = false;
            // compilerProcess.StartInfo.RedirectStandardOutput = true;
            // compilerProcess.StartInfo.RedirectStandardError = true;
            // compilerProcess.StartInfo.CreateNoWindow = true;
            // compilerProcess.Start();

            // while (!compilerProcess.StandardOutput.EndOfStream)
            // {
            //     string currentLine = compilerProcess.StandardOutput.ReadLine();
            //     Console.WriteLine(currentLine);
            // }

            // compilerProcess.WaitForExit();
        }
    }

    static void Main(string[] arguments)
    {
        CommandRunner commandRunner = new CommandRunner();
        if (arguments.Length != 3)
        {
            commandRunner.LogMessage(CommandRunner.LogLevel.Error, "Invalid arguments!");
            return;
        }

        CommandRunner.BuildPlatform buildPlatform = CommandRunner.BuildPlatform.Windows;
        if (String.Equals(arguments[1], "Linux", StringComparison.OrdinalIgnoreCase))
        {
            buildPlatform = CommandRunner.BuildPlatform.Linux;
        }
        else if (String.Equals(arguments[1], "MacOS", StringComparison.OrdinalIgnoreCase))
        {
            buildPlatform = CommandRunner.BuildPlatform.MacOS;
        }

        CommandRunner.BuildType buildType = CommandRunner.BuildType.Release;
        if (String.Equals(arguments[2], "Debug", StringComparison.OrdinalIgnoreCase))
        {
            buildType = CommandRunner.BuildType.Debug;
        }

        switch (buildPlatform)
        {
            case CommandRunner.BuildPlatform.Windows:
            {
                commandRunner.WindowsBuild(arguments[0], buildPlatform, buildType);
                break;
            }
            case CommandRunner.BuildPlatform.MacOS:
            {
                commandRunner.MacOSBuild(arguments[0], buildPlatform, buildType);
                break;
            }
        }

        return;
    }
}

