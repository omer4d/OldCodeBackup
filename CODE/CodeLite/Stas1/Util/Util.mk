##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=Util
ConfigurationName      :=Debug
WorkspacePath          := "D:\Stuff\CODE\CodeLite\Stas1"
ProjectPath            := "D:\Stuff\CODE\CodeLite\Stas1\Util"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Stas
Date                   :=12/07/13
CodeLitePath           :="D:\Programs\CodeLite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/lib$(ProjectName).a
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="Util.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=windres
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -g $(Preprocessors)
CFLAGS   :=  -g $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=D:\Programs\CodeLite
UNIT_TEST_PP_SRC_DIR:=C:\UnitTest++-1.3
PATH:=$(PATH);C:\MinGW\bin
Objects0=$(IntermediateDirectory)/AABB$(ObjectSuffix) $(IntermediateDirectory)/DrawVec2f$(ObjectSuffix) $(IntermediateDirectory)/ImageLoader$(ObjectSuffix) $(IntermediateDirectory)/Logger$(ObjectSuffix) $(IntermediateDirectory)/Mat9f$(ObjectSuffix) $(IntermediateDirectory)/SAT$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(IntermediateDirectory) $(OutputFile)

$(OutputFile): $(Objects)
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(AR) $(ArchiveOutputSwitch)$(OutputFile) @$(ObjectsFileList) $(ArLibs)
	@$(MakeDirCommand) "D:\Stuff\CODE\CodeLite\Stas1/.build-debug"
	@echo rebuilt > "D:\Stuff\CODE\CodeLite\Stas1/.build-debug/Util"

./Debug:
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/AABB$(ObjectSuffix): AABB.cpp $(IntermediateDirectory)/AABB$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/Util/AABB.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/AABB$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/AABB$(DependSuffix): AABB.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/AABB$(ObjectSuffix) -MF$(IntermediateDirectory)/AABB$(DependSuffix) -MM "AABB.cpp"

$(IntermediateDirectory)/AABB$(PreprocessSuffix): AABB.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/AABB$(PreprocessSuffix) "AABB.cpp"

$(IntermediateDirectory)/DrawVec2f$(ObjectSuffix): DrawVec2f.cpp $(IntermediateDirectory)/DrawVec2f$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/Util/DrawVec2f.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/DrawVec2f$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/DrawVec2f$(DependSuffix): DrawVec2f.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/DrawVec2f$(ObjectSuffix) -MF$(IntermediateDirectory)/DrawVec2f$(DependSuffix) -MM "DrawVec2f.cpp"

$(IntermediateDirectory)/DrawVec2f$(PreprocessSuffix): DrawVec2f.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/DrawVec2f$(PreprocessSuffix) "DrawVec2f.cpp"

$(IntermediateDirectory)/ImageLoader$(ObjectSuffix): ImageLoader.cpp $(IntermediateDirectory)/ImageLoader$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/Util/ImageLoader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ImageLoader$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ImageLoader$(DependSuffix): ImageLoader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ImageLoader$(ObjectSuffix) -MF$(IntermediateDirectory)/ImageLoader$(DependSuffix) -MM "ImageLoader.cpp"

$(IntermediateDirectory)/ImageLoader$(PreprocessSuffix): ImageLoader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ImageLoader$(PreprocessSuffix) "ImageLoader.cpp"

$(IntermediateDirectory)/Logger$(ObjectSuffix): Logger.cpp $(IntermediateDirectory)/Logger$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/Util/Logger.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Logger$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Logger$(DependSuffix): Logger.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Logger$(ObjectSuffix) -MF$(IntermediateDirectory)/Logger$(DependSuffix) -MM "Logger.cpp"

$(IntermediateDirectory)/Logger$(PreprocessSuffix): Logger.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Logger$(PreprocessSuffix) "Logger.cpp"

$(IntermediateDirectory)/Mat9f$(ObjectSuffix): Mat9f.cpp $(IntermediateDirectory)/Mat9f$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/Util/Mat9f.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Mat9f$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Mat9f$(DependSuffix): Mat9f.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Mat9f$(ObjectSuffix) -MF$(IntermediateDirectory)/Mat9f$(DependSuffix) -MM "Mat9f.cpp"

$(IntermediateDirectory)/Mat9f$(PreprocessSuffix): Mat9f.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Mat9f$(PreprocessSuffix) "Mat9f.cpp"

$(IntermediateDirectory)/SAT$(ObjectSuffix): SAT.cpp $(IntermediateDirectory)/SAT$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/Util/SAT.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SAT$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SAT$(DependSuffix): SAT.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SAT$(ObjectSuffix) -MF$(IntermediateDirectory)/SAT$(DependSuffix) -MM "SAT.cpp"

$(IntermediateDirectory)/SAT$(PreprocessSuffix): SAT.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SAT$(PreprocessSuffix) "SAT.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/AABB$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/AABB$(DependSuffix)
	$(RM) $(IntermediateDirectory)/AABB$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/DrawVec2f$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/DrawVec2f$(DependSuffix)
	$(RM) $(IntermediateDirectory)/DrawVec2f$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ImageLoader$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ImageLoader$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ImageLoader$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/Logger$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/Logger$(DependSuffix)
	$(RM) $(IntermediateDirectory)/Logger$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/Mat9f$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/Mat9f$(DependSuffix)
	$(RM) $(IntermediateDirectory)/Mat9f$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/SAT$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/SAT$(DependSuffix)
	$(RM) $(IntermediateDirectory)/SAT$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile)
	$(RM) "../.build-debug/Util"


