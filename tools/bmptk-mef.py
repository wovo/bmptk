# ===========================================================================
# 
# BMPTK's Make Editor Files:
# create the Codelite files for a set of projects,
# which can be native or bmptk
#
# (c) Wouter van Ooijen (wouter@voti.nl)
# 2016-04-15 version 1.0 work-in-progress
# license: Boost Software License - Version 1.0
#
# ===========================================================================
#
# Known limitations
# 
# tested only on Windows
# works only for one level of subdirectories
#
# ===========================================================================


from __future__ import print_function
import sys, os, argparse, shutil

def file_from_text( file_name, text ):
   "write the string text to the file_name"
   f = open( file_name, "w" )
   f.write( text )
   f.close() 
   
def remove_by_extension( dir, ext ):
   files = os.listdir( dir )
   for file in files:
     if file.endswith( ext ):
        os.remove( os.path.join( dir, file ))   

def remove( dir, file ):
   try:
      os.remove( os.path.join( dir, file ))   
   except:
      pass   

class projectdir:
   "a single project directory"

   def __init__( self, target, path, subdir, files ):
      self.target = target
      self.path = path
      self.subdir = subdir
      self.files = files   
      self.main = None
      self.is_project = 0
      self.name = subdir

      for file in self.files:
         if file in [ "main.c", "main.cpp" ]:
            self.is_project = 1
            self.main = file            
      
   def make_codelite( self, codelite, workspace ):
      file_from_text(
         os.path.join( self.path, self.subdir, codelite ),
         codelite_project_file( self.name, self.main, self.files, self.target )
      );
      file_from_text(
         os.path.join( self.path, self.subdir, "_run.bat" ),
         "bmptk-make run\n"
      );
      if 0: file_from_text(
         os.path.join( self.path, self.subdir, workspace ),
         codelite_workspace_file( [ self.name ], codelite, 1, self.target )
      );         
      
   def make_files( self, codelite, workspace ):
      if 0: print( "create project %s in %s" % ( self.name, self.subdir ) ) 
      print( "create project %s " % ( self.name ) )         
      self.make_codelite( codelite, workspace )
      
   def cleanup( self ):
      dir = self.subdir
      shutil.rmtree( os.path.join( dir, "Debug" ), ignore_errors = True )  
      shutil.rmtree( os.path.join( dir, "Release" ), ignore_errors = True )  
      shutil.rmtree( os.path.join( dir, "html" ), ignore_errors = True )  
      shutil.rmtree( os.path.join( dir, ".clang" ), ignore_errors = True )  
      shutil.rmtree( os.path.join( dir, ".codelite" ), ignore_errors = True )  
      
      remove_by_extension( dir, ".o" )
      remove_by_extension( dir, ".bin" )
      remove_by_extension( dir, ".elf" )
      remove_by_extension( dir, ".ld" )
      remove_by_extension( dir, ".map" )
      remove_by_extension( dir, ".ppr" )
      remove_by_extension( dir, ".workspace" )
      remove_by_extension( dir, ".project" )
      remove( dir, "bmptk_fixed_size_stack.c" )
      
      files = os.listdir( dir )
      for file in files:
         if file.endswith( ".mk" ):
           os.remove( os.path.join( dir, file ))       
           try:           
              os.remove( 
                 os.path.join( dir, file.replace( ".mk", ".txt" )))        
           except:
              pass           

# =========================================================================== 

def arguments_parser():
   parser = argparse.ArgumentParser( 
      description = \
      'Create the editor files for CodeLite for the '
      'projects in a set of subdirectories, '
      'which can be native or bmptk '
      '(identified by the presence of a makefile). '
   )       
   parser.add_argument(
      '-os',      
      help = 'the OS: windows or linux' )
   parser.add_argument(
      '-root', 
      default = '.',      
      help = 'the directory that contains the subdirectory projects. '
             'default: the current directory.' )
   parser.add_argument(
      '-codelite_project', 
      default = '_codelite.project',      
      help = 'the name of the CodeLite project file created in each '
             'project directory.' )
   parser.add_argument(
      '-codelite_workspace', 
      default = '__codelite.workspace',      
      help = 'the name of the CodeLite workspace file created in each '
             'project directory and in the root directory.' )
 
   return parser
   
def entries( path ):
   try:
      return os.listdir( path )
   except:
      return []         

def create_files(  
   target,
   root, 
   codelite_project, 
   codelite_workspace
):
   shutil.rmtree( os.path.join( "", ".clang" ), ignore_errors = True )  
   shutil.rmtree( os.path.join( "", ".codelite" ), ignore_errors = True )  
   
   projects = []
   for subdir in os.listdir( root ):
      files = entries( os.path.join( root, subdir ) )
      # print( root, subdir, files )
      if files:
         projects.append( projectdir( target, root, subdir, files ) )
         
   names = []
   for project in projects:
      if project.is_project:
         project.cleanup();
         project.make_files( codelite_project, codelite_workspace )
         names.append( project.name )
   file_from_text(
      os.path.join( root, codelite_workspace ),
         codelite_workspace_file( names, codelite_project, 0, target )
      );              

def run():
   parser = arguments_parser()
   results = parser.parse_args()
   create_files(  
	  results.os,
      results.root, 
      results.codelite_project,
      results.codelite_workspace
   )
   

# =========================================================================== 

def add_to_edit_files( file ):
   if file == "main.cpp": return 0
   if file.find( ".cpp" ) >= 0: return 1
   if file.find( ".hpp" ) >= 0: return 1
   if file.find( ".py" ) >= 0: return 1
   return 0
   
def has_makefile( list ):
   for item in list:
      if item in [ 'Makefile', 'makefile' ]:
         return 1
   return 0   

def codelite_project_file( name, main, files, target ):
   print( files )
   if has_makefile( files ):
      print( "bmptk" )
      s = codelite_project_template_bmptk()
   else:
      s = codelite_project_template_mingw()      
   s = s.replace( "%%MAIN%%", main ).replace( "%%NAME%%", name )
   if target == "windows":
      replacement = 'Command="_run.bat" CommandArguments="run"'
   else:		 
      replacement = 'Command="gnome-terminal" CommandArguments="-- bash -c \'sudo make run\'"'
   s = s.replace( "%%RUN%%", replacement )   
   for file in files:
      if( add_to_edit_files( file ) ):
         # print( file )
         f = '\n    <File Name="%%SOURCE%%"/>%%FILES%%'.replace( 
            "%%SOURCE%%", file )
         s = s.replace( "%%FILES%%", f )           
   s = s.replace( "%%FILES%%", "" )
   return s.replace( "\n\n", "\n" )

def codelite_project_template_bmptk():
   return """<?xml version="1.0" encoding="UTF-8"?>
<CodeLite_Project Name="%%NAME%%" InternalType="">
  <Plugins>
    <Plugin Name="qmake">
      <![CDATA[00020001N0005Debug0000000000000001N0007Release000000000000]]>
    </Plugin>
    <Plugin Name="CMakePlugin">
      <![CDATA[[{
  "name": "Debug",
  "enabled": false,
  "buildDirectory": "build",
  "sourceDirectory": "$(ProjectPath)",
  "generator": "",
  "buildType": "",
  "arguments": [],
  "parentProject": ""
 }, {
  "name": "Release",
  "enabled": false,
  "buildDirectory": "build",
  "sourceDirectory": "$(ProjectPath)",
  "generator": "",
  "buildType": "",
  "arguments": [],
  "parentProject": ""
 }]]]>
    </Plugin>
  </Plugins>
  <Description/>
  <Dependencies/>
  <VirtualDirectory Name="source">
    <File Name="%%MAIN%%"/>%%FILES%%
    <File Name="Makefile"/>
  </VirtualDirectory>
  <Settings Type="Dynamic Library">
    <GlobalSettings>
      <Compiler Options="" C_Options="" Assembler="">
        <IncludePath Value="."/>
      </Compiler>
      <Linker Options="">
        <LibraryPath Value="."/>
      </Linker>
      <ResourceCompiler Options=""/>
    </GlobalSettings>
    <Configuration Name="Debug" CompilerType="MinGW ( TDM-GCC-32 )" DebuggerType="GNU gdb debugger" Type="Executable" BuildCmpWithGlobalSettings="append" BuildLnkWithGlobalSettings="append" BuildResWithGlobalSettings="append">
       <Compiler Options="-g;-O0;-std=c++11" C_Options="-g;-O0;-Wall" Assembler="" Required="no" PreCompiledHeader="" PCHInCommandLine="no" PCHFlags="" PCHFlagsPolicy="0">
        <IncludePath Value=".;../Catch/include"/>
      </Compiler>
      <Linker Options="" Required="yes"/>
      <ResourceCompiler Options="" Required="no"/>
      <General OutputFile="" IntermediateDirectory="./Debug" %%RUN%% UseSeparateDebugArgs="no" DebugArguments="" WorkingDirectory="$(ProjectPath)" PauseExecWhenProcTerminates="no" IsGUIProgram="no" IsEnabled="yes"/>
      <Environment EnvVarSetName="&lt;Use Defaults&gt;" DbgSetName="&lt;Use Defaults&gt;">
        <![CDATA[]]>
      </Environment>
      <Debugger IsRemote="no" RemoteHostName="" RemoteHostPort="" DebuggerPath="" IsExtended="no">
        <DebuggerSearchPaths/>
        <PostConnectCommands/>
        <StartupCommands/>
      </Debugger>
      <PreBuild/>
      <PostBuild/>
      <CustomBuild Enabled="yes">
        <RebuildCommand>bmptk-make clean build</RebuildCommand>
        <CleanCommand>bmptk-make clean</CleanCommand>
        <BuildCommand>bmptk-make build</BuildCommand>
        <PreprocessFileCommand/>
        <SingleFileCommand/>
        <MakefileGenerationCommand/>
        <ThirdPartyToolName>None</ThirdPartyToolName>
        <WorkingDirectory>$(ProjectPath)</WorkingDirectory>
      </CustomBuild>
      <AdditionalRules>
        <CustomPostBuild/>
        <CustomPreBuild/>
      </AdditionalRules>
      <Completion EnableCpp11="no" EnableCpp14="no">
        <ClangCmpFlagsC/>
        <ClangCmpFlags/>
        <ClangPP/>
        <SearchPaths/>
      </Completion>
    </Configuration>
    <Configuration Name="Release" CompilerType="MinGW ( TDM-GCC-32 )" DebuggerType="GNU gdb debugger" Type="Dynamic Library" BuildCmpWithGlobalSettings="append" BuildLnkWithGlobalSettings="append" BuildResWithGlobalSettings="append">
      <Compiler Options="" C_Options="" Assembler="" Required="no" PreCompiledHeader="" PCHInCommandLine="no" PCHFlags="" PCHFlagsPolicy="0">
        <IncludePath Value="."/>
      </Compiler>
      <Linker Options="-O2" Required="yes"/>
      <ResourceCompiler Options="" Required="no"/>
      <General OutputFile="" IntermediateDirectory="./Release" %%RUN%% UseSeparateDebugArgs="no" DebugArguments="" WorkingDirectory="$(ProjectPath)" PauseExecWhenProcTerminates="no" IsGUIProgram="no" IsEnabled="yes"/>
      <Environment EnvVarSetName="&lt;Use Defaults&gt;" DbgSetName="&lt;Use Defaults&gt;">
        <![CDATA[]]>
      </Environment>
      <Debugger IsRemote="no" RemoteHostName="" RemoteHostPort="" DebuggerPath="" IsExtended="no">
        <DebuggerSearchPaths/>
        <PostConnectCommands/>
        <StartupCommands/>
      </Debugger>
      <PreBuild/>
      <PostBuild/>
      <CustomBuild Enabled="yes">
        <RebuildCommand>bmptk-make clean build</RebuildCommand>
        <CleanCommand>bmptk-make clean</CleanCommand>
        <BuildCommand>bmptk-make build</BuildCommand>
        <PreprocessFileCommand/>
        <SingleFileCommand/>
        <MakefileGenerationCommand/>
        <ThirdPartyToolName>None</ThirdPartyToolName>
        <WorkingDirectory>$(ProjectPath)</WorkingDirectory>
      </CustomBuild>
      <AdditionalRules>
        <CustomPostBuild/>
        <CustomPreBuild/>
      </AdditionalRules>
      <Completion EnableCpp11="no" EnableCpp14="no">
        <ClangCmpFlagsC/>
        <ClangCmpFlags/>
        <ClangPP/>
        <SearchPaths/>
      </Completion>
    </Configuration>
  </Settings>
</CodeLite_Project>   
"""   

def codelite_project_template_mingw():
   return """<?xml version="1.0" encoding="UTF-8"?>
<CodeLite_Project Name="%%NAME%%" InternalType="Console">
  <Plugins>
    <Plugin Name="qmake">
      <![CDATA[00010001N0005Debug000000000000]]>
    </Plugin>
    <Plugin Name="CMakePlugin">
      <![CDATA[[{
  "name": "Debug",
  "enabled": false,
  "buildDirectory": "build",
  "sourceDirectory": "$(ProjectPath)",
  "generator": "",
  "buildType": "",
  "arguments": [],
  "parentProject": ""
 }]]]>
    </Plugin>
  </Plugins>
  <Description/>
  <Dependencies/>
  <VirtualDirectory Name="src">
    <File Name="%%MAIN%%"/>%%FILES%%
  </VirtualDirectory>
  <Settings Type="Executable">
    <GlobalSettings>
      <Compiler Options="" C_Options="" Assembler="">
        <IncludePath Value="."/>
      </Compiler>
      <Linker Options="">
        <LibraryPath Value="."/>
      </Linker>
      <ResourceCompiler Options=""/>
    </GlobalSettings>
    <Configuration Name="Debug" CompilerType="gnu g++" DebuggerType="GNU gdb debugger" Type="Executable" BuildCmpWithGlobalSettings="append" BuildLnkWithGlobalSettings="append" BuildResWithGlobalSettings="append">
      <Compiler Options="-g;-O0;-std=c++11" C_Options="-g;-O0;-Wall" Assembler="" Required="yes" PreCompiledHeader="" PCHInCommandLine="no" PCHFlags="" PCHFlagsPolicy="0">
        <IncludePath Value=".;../Catch/include"/>
      </Compiler>
      <Linker Options="-lgdi32 -static-libgcc -static-libstdc++ " Required="yes"/>
      <ResourceCompiler Options="" Required="no"/>
      <General OutputFile="$(IntermediateDirectory)/$(ProjectName)" IntermediateDirectory="./Debug" Command="./$(ProjectName)" CommandArguments="" UseSeparateDebugArgs="no" DebugArguments="" WorkingDirectory="$(IntermediateDirectory)" PauseExecWhenProcTerminates="yes" IsGUIProgram="no" IsEnabled="yes"/>
      <Environment EnvVarSetName="&lt;Use Defaults&gt;" DbgSetName="&lt;Use Defaults&gt;">
        <![CDATA[]]>
      </Environment>
      <Debugger IsRemote="no" RemoteHostName="" RemoteHostPort="" DebuggerPath="" IsExtended="yes">
        <DebuggerSearchPaths/>
        <PostConnectCommands/>
        <StartupCommands/>
      </Debugger>
      <PreBuild/>
      <PostBuild/>
      <CustomBuild Enabled="no">
        <RebuildCommand/>
        <CleanCommand/>
        <BuildCommand/>
        <PreprocessFileCommand/>
        <SingleFileCommand/>
        <MakefileGenerationCommand/>
        <ThirdPartyToolName>None</ThirdPartyToolName>
        <WorkingDirectory/>
      </CustomBuild>
      <AdditionalRules>
        <CustomPostBuild/>
        <CustomPreBuild/>
      </AdditionalRules>
      <Completion EnableCpp11="no" EnableCpp14="no">
        <ClangCmpFlagsC/>
        <ClangCmpFlags/>
        <ClangPP/>
        <SearchPaths/>
      </Completion>
    </Configuration>
    <Configuration Name="Release" CompilerType="MinGW ( TDM-GCC-32 )" DebuggerType="GNU gdb debugger" Type="Executable" BuildCmpWithGlobalSettings="append" BuildLnkWithGlobalSettings="append" BuildResWithGlobalSettings="append">
      <Compiler Options="-O2;-Wall" C_Options="-O2;-Wall" Assembler="" Required="yes" PreCompiledHeader="" PCHInCommandLine="no" PCHFlags="" PCHFlagsPolicy="0">
        <IncludePath Value="."/>
        <Preprocessor Value="NDEBUG"/>
      </Compiler>
      <Linker Options="" Required="yes"/>
      <ResourceCompiler Options="" Required="no"/>
      <General OutputFile="$(IntermediateDirectory)/$(ProjectName)" IntermediateDirectory="./Release" Command="./$(ProjectName)" CommandArguments="" UseSeparateDebugArgs="no" DebugArguments="" WorkingDirectory="$(IntermediateDirectory)" PauseExecWhenProcTerminates="yes" IsGUIProgram="no" IsEnabled="yes"/>
      <Environment EnvVarSetName="&lt;Use Defaults&gt;" DbgSetName="&lt;Use Defaults&gt;">
        <![CDATA[]]>
      </Environment>
      <Debugger IsRemote="no" RemoteHostName="" RemoteHostPort="" DebuggerPath="" IsExtended="yes">
        <DebuggerSearchPaths/>
        <PostConnectCommands/>
        <StartupCommands/>
      </Debugger>
      <PreBuild/>
      <PostBuild/>
      <CustomBuild Enabled="no">
        <RebuildCommand/>
        <CleanCommand/>
        <BuildCommand/>
        <PreprocessFileCommand/>
        <SingleFileCommand/>
        <MakefileGenerationCommand/>
        <ThirdPartyToolName>None</ThirdPartyToolName>
        <WorkingDirectory/>
      </CustomBuild>
      <AdditionalRules>
        <CustomPostBuild/>
        <CustomPreBuild/>
      </AdditionalRules>
      <Completion EnableCpp11="no" EnableCpp14="no">
        <ClangCmpFlagsC/>
        <ClangCmpFlags/>
        <ClangPP/>
        <SearchPaths/>
      </Completion>
    </Configuration>
  </Settings>
</CodeLite_Project>"""   

# =========================================================================== 

def codelite_workspace_file( names, file, local, target ):
   list = ""
   for name in names:
      p = codelite_workspace_project_template()
      list += p.replace( "%%NAME%%", name ).replace( "%%FILE%%", file )
   s = codelite_workspace_template().replace( "%%PROJECTS%%", list )      
   return s.replace( "\n\n", "\n" )
   
def codelite_workspace_project_template():
   return """  <Project Name="%%NAME%%" Path="%%NAME%%/%%FILE%%" Active="No"/>
  <BuildMatrix>
    <WorkspaceConfiguration Name="Release" Selected="yes">
      <Project Name="%%NAME%%" ConfigName="Release"/>
    </WorkspaceConfiguration>
  </BuildMatrix> 
"""   

def codelite_workspace_template():
   return """<?xml version="1.0" encoding="UTF-8"?>
<CodeLite_Workspace Name="example code" Database="">
%%PROJECTS%%
</CodeLite_Workspace>
"""

# =========================================================================== 


run()