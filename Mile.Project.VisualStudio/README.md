# Mile.Project.VisualStudio

Mile.Project.VisualStudio is a configuration template for defining Visual
Studio (MSBuild) C++ projects. It can help us to simplify the definition of 
Visual Studio (MSBuild) C++ projects.

Mile.Project.VisualStudio is also one of the Mile.Cpp family project.

## Files

```
- Mile.Project.VisualStudio
  - Mile.Project.Cpp.props
  - Mile.Project.Cpp.targets
  - Mile.Project.Cpp.VC-LTL.props
  - Mile.Project.Manifest.rc
  - Mile.Project.Version.h
  - Mile.Project.Version.rc
- .editorconfig
- .gitignore
- BuildAllTargets.cmd
- BuildAllTargets.proj
```

## Usages

### How to define the project type in the "Globals" label property group.

- If the project is a console application project.
  Please use `<MileProjectType>ConsoleApplication</MileProjectType>`.
- If the project is a windows application project
  Please use `<MileProjectType>WindowsApplication</MileProjectType>`.
- If the project is a dynamic library project.
  Please use `<MileProjectType>DynamicLibrary</MileProjectType>`.
- If the project is a static library project.
  Please use `<MileProjectType>StaticLibrary</MileProjectType>`.
- If the project is a UEFI application project.
  Please use `<MileProjectType>UefiApplication</MileProjectType>`.

### How to define the manifest file in the "Globals" label property group.

Please use 
`<MileProjectManifestFile>C:\Folder\Manifest.manifest</MileProjectManifestFile>`

### How to enable the version information support.

Copy Mile.Project.Properties.Template.h to the project folder and rename it to
Mile.Project.Properties.h.

## Example

The project in [SimpleProject](SimpleProject) folder is a template Windows 
application project for integrating the Mile.Project.VisualStudio with 
[VC-LTL](https://github.com/Chuyu-Team/VC-LTL) support.
