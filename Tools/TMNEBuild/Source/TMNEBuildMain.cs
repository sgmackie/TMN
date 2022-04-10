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
            public Dictionary<string, string[]> CompileFlags;
            public List<string> SourceFiles;
            public List<string> IncludePaths;
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

            settings.CompileFlags = new Dictionary<string, string[]>();
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
                    flagList[i] = " " + flags[i].ToString();                   
                }

                settings.CompileFlags.Add(flagGroup.Key.ToString(), flagList);
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
                    string[] sourceFiles = Directory.GetFiles(moduleRootPath, "*.cpp", SearchOption.AllDirectories);
                    settings.SourceFiles.AddRange(sourceFiles);
                    settings.IncludePaths.Add(moduleRootPath);
                }
            }

            // Scan for this project
            string[] projectSourceFiles = Directory.GetFiles(rootPath, "*.cpp", SearchOption.AllDirectories);
            settings.SourceFiles.AddRange(projectSourceFiles);
            settings.IncludePaths.Add(rootPath);

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

        public void Run(string projectPath, BuildPlatform buildPlatform, BuildType buildType)
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

            string batFilePath = Path.Combine(executablePath, "WindowsCompile.bat");
            string argumentList = "cl";

            foreach (var flags in buildSettings.CompileFlags)
            {
                foreach (var flag in flags.Value)
                {
                    argumentList += flag;
                }
            }

            string outputList = "";
            if (buildSettings.Type == BuildSettings.UnitType.Project)
            {
                string baseOutputBath = buildSettings.OutputPath + Path.DirectorySeparatorChar + buildType.ToString() + Path.DirectorySeparatorChar;
                Directory.CreateDirectory(baseOutputBath);
                outputList += " /Fe" + baseOutputBath + buildSettings.Title + ".exe";
                outputList += " /Fo" + baseOutputBath;
                outputList += " /Fd" + baseOutputBath + " ";
            }

            string fileList = "";
            foreach (string file in buildSettings.SourceFiles)
            {
                fileList += file + " ";
            }

            string includeList = "";
            foreach (string path in buildSettings.IncludePaths)
            {
                includeList += "/I" + path + " ";
            }

            LogMessage(LogLevel.Info, buildSettings.Type.ToString() + " " + buildSettings.Title + " (" + buildPlatform.ToString() + " - " + buildType.ToString() + ")");
            LogMessage(LogLevel.Info, argumentList);

            Process compilerProcess = new Process();
            compilerProcess.StartInfo.FileName = "cmd.exe";
            compilerProcess.StartInfo.Arguments = "/c call " + argumentList + outputList + includeList + fileList;
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
    }

    static void Main(string[] arguments)
    {
        CommandRunner commandRunner = new CommandRunner();
        if (arguments.Length != 3)
        {
            commandRunner.LogMessage(CommandRunner.LogLevel.Error, "Invalid arguments!");
            return;
        }

        CommandRunner.BuildType buildType = CommandRunner.BuildType.Release;
        if (String.Equals(arguments[1], "Debug", StringComparison.OrdinalIgnoreCase))
        {
            buildType = CommandRunner.BuildType.Debug;
        }

        CommandRunner.BuildPlatform buildPlatform = CommandRunner.BuildPlatform.Windows;
        if (String.Equals(arguments[1], "Linux", StringComparison.OrdinalIgnoreCase))
        {
            buildPlatform = CommandRunner.BuildPlatform.Linux;
        }

        commandRunner.Run(arguments[0], buildPlatform, buildType);

        return;
    }
}

