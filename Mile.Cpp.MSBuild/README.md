# Mile.Project.Cpp.VisualStudio

Mile.Project.Cpp.VisualStudio is a configuration template for defining Visual
Studio (MSBuild) C++ projects. It can help us to simplify the definition of 
Visual Studio (MSBuild) C++ projects.

Mile.Project.Cpp.VisualStudio is also one of the Mile.Cpp family project.

## Files

```
- Mile.Project
  - Mile.Project.Cpp.props
  - Mile.Project.Cpp.targets
  - Mile.Project.Cpp.VC-LTL.props
  - Mile.Project.Manifest.rc
  - Mile.Project.Properties.Template.h
  - Mile.Project.Version.h
  - Mile.Project.Version.rc
- .editorconfig
- .gitignore
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

### How to define the manifest file in the "Globals" label property group.

Please use 
`<MileProjectManifestFile>C:\Folder\Manifest.manifest</MileProjectManifestFile>`

### How to enable the version information support.

Copy Mile.Project.Properties.Template.h to the project folder and rename it to
Mile.Project.Properties.h.

## Example

Here is a template project for integrating the Mile.Project.Cpp.VisualStudio 
with [VC-LTL](https://github.com/Chuyu-Team/VC-LTL) support.

P.S. You need to copy the files mentioned in [Files](#Files) to 
`{YourProjectRoot}` directory first.

### `{YourProjectRoot}`\SimpleProject.vcxproj

```
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup Label="Globals">
    <ProjectGuid>{F3E82C07-D4FD-45AD-9C7C-29C7FC210158}</ProjectGuid>
    <RootNamespace>SimpleProject</RootNamespace>
    <MileProjectType>ConsoleApplication</MileProjectType>
    <MileProjectManifestFile>SimpleProject.manifest</MileProjectManifestFile>
  </PropertyGroup>
  <Import Project="..\Mile.Project\Mile.Project.Cpp.props" />
  <Import Project="..\Mile.Project\Mile.Project.Cpp.VC-LTL.props" />
  <ItemGroup>
    <ClCompile Include="SimpleProject.cpp" />
  </ItemGroup>
  <ItemGroup>
    <Manifest Include="SimpleProject.manifest" />
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="Mile.Project.Properties.h" />
  </ItemGroup>
  <Import Project="..\Mile.Project\Mile.Project.Cpp.targets" />
</Project>
```

### `{YourProjectRoot}`\SimpleProject.vcxproj.filters

```
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <ClCompile Include="SimpleProject.cpp" />
  </ItemGroup>
  <ItemGroup>
    <Manifest Include="SimpleProject.manifest" />
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="Mile.Project.Properties.h" />
  </ItemGroup>
</Project>
```

### `{YourProjectRoot}`\SimpleProject.vcxproj.user

```
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup />
</Project>
```

### `{YourProjectRoot}`\SimpleProject.manifest

```
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<assembly manifestVersion="1.0" xmlns="urn:schemas-microsoft-com:asm.v1">
	<trustInfo xmlns="urn:schemas-microsoft-com:asm.v2">
		<security>
			<requestedPrivileges>
				<requestedExecutionLevel level="asInvoker" uiAccess="false"/>
			</requestedPrivileges>
		</security>
	</trustInfo>
	<compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1">
		<application>
			<supportedOS Id="{e2011457-1546-43c5-a5fe-008deee3d3f0}"/>
			<supportedOS Id="{35138b9a-5d96-4fbd-8e2d-a2440225f93a}"/>
			<supportedOS Id="{4a2f28e3-53b9-4441-ba9c-d69d4a4a6e38}"/>
			<supportedOS Id="{1f676c76-80e1-4239-95bb-83d0f6d0da78}"/>
			<supportedOS Id="{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}"/>
		</application>
	</compatibility>
</assembly>
```

### `{YourProjectRoot}`\Mile.Project.Properties.h (Should be saved with UTF-16 LE)

```
/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Project.Properties.h
 * PURPOSE:   Project Properties for projects with Mile.Project
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#ifndef MILE_PROJECT_PROPERTIES
#define MILE_PROJECT_PROPERTIES

#define MILE_PROJECT_VERSION_MAJOR 1
#define MILE_PROJECT_VERSION_MINOR 0
#define MILE_PROJECT_VERSION_PATCH 0
#define MILE_PROJECT_VERSION_REVISION 0

//#define MILE_PROJECT_VERSION_TAG L"Alpha 1"

#define MILE_PROJECT_COMPANY_NAME \
    "The Simple Project company"
#define MILE_PROJECT_FILE_DESCRIPTION \
    "Simple Project"
#define MILE_PROJECT_INTERNAL_NAME \
    "SimpleProject"
#define MILE_PROJECT_LEGAL_COPYRIGHT \
    "© The Simple Project company. All rights reserved."
#define MILE_PROJECT_ORIGINAL_FILENAME \
    "SimpleProject.exe"
#define MILE_PROJECT_PRODUCT_NAME \
    "SimpleProject"

#endif

#include <Mile.Project.Version.h>
```

### `{YourProjectRoot}`\SimpleProject.cpp

```
#include <clocale>
#include <cstdio>
#include <cwchar>

int main()
{
    // I can't use that because of the limitation in VC-LTL.
    // std::setlocale(LC_ALL, "zh_CN.UTF-8");

    std::setlocale(LC_ALL, "chs");

    std::wprintf(
        L"VC-LTL Samples - VisualStudioConsoleDemo\n"
        L"================================\n"
        L"他喵的 MSVC 2019 工具集坑了我们价值百万美刀的项目！\n"
        L"The F@cking MSVC 2019 toolset ruined our $1M project!\n");

    return 0;
}
```
