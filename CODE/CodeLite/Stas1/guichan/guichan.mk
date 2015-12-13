##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=guichan
ConfigurationName      :=Release
WorkspacePath          := "D:\Stuff\CODE\CodeLite\Stas1"
ProjectPath            := "D:\Stuff\CODE\CodeLite\Stas1\guichan"
IntermediateDirectory  :=./Release
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
ObjectsFileList        :="guichan.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=windres
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)$(WorkspacePath)/guichan/include 
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
CXXFLAGS :=   $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=D:\Programs\CodeLite
UNIT_TEST_PP_SRC_DIR:=C:\UnitTest++-1.3
PATH:=$(PATH);C:\MinGW\bin
Objects0=$(IntermediateDirectory)/src_actionevent$(ObjectSuffix) $(IntermediateDirectory)/src_basiccontainer$(ObjectSuffix) $(IntermediateDirectory)/src_cliprectangle$(ObjectSuffix) $(IntermediateDirectory)/src_color$(ObjectSuffix) $(IntermediateDirectory)/src_defaultfont$(ObjectSuffix) $(IntermediateDirectory)/src_event$(ObjectSuffix) $(IntermediateDirectory)/src_exception$(ObjectSuffix) $(IntermediateDirectory)/src_focushandler$(ObjectSuffix) $(IntermediateDirectory)/src_font$(ObjectSuffix) $(IntermediateDirectory)/src_genericinput$(ObjectSuffix) \
	$(IntermediateDirectory)/src_graphics$(ObjectSuffix) $(IntermediateDirectory)/src_gui$(ObjectSuffix) $(IntermediateDirectory)/src_guichan$(ObjectSuffix) $(IntermediateDirectory)/src_image$(ObjectSuffix) $(IntermediateDirectory)/src_imagefont$(ObjectSuffix) $(IntermediateDirectory)/src_inputevent$(ObjectSuffix) $(IntermediateDirectory)/src_key$(ObjectSuffix) $(IntermediateDirectory)/src_keyevent$(ObjectSuffix) $(IntermediateDirectory)/src_keyinput$(ObjectSuffix) $(IntermediateDirectory)/src_mouseevent$(ObjectSuffix) \
	$(IntermediateDirectory)/src_mouseinput$(ObjectSuffix) $(IntermediateDirectory)/src_rectangle$(ObjectSuffix) $(IntermediateDirectory)/src_selectionevent$(ObjectSuffix) $(IntermediateDirectory)/src_widget$(ObjectSuffix) $(IntermediateDirectory)/widgets_button$(ObjectSuffix) $(IntermediateDirectory)/widgets_checkbox$(ObjectSuffix) $(IntermediateDirectory)/widgets_container$(ObjectSuffix) $(IntermediateDirectory)/widgets_dropdown$(ObjectSuffix) $(IntermediateDirectory)/widgets_icon$(ObjectSuffix) $(IntermediateDirectory)/widgets_imagebutton$(ObjectSuffix) \
	$(IntermediateDirectory)/widgets_label$(ObjectSuffix) $(IntermediateDirectory)/widgets_listbox$(ObjectSuffix) $(IntermediateDirectory)/widgets_radiobutton$(ObjectSuffix) $(IntermediateDirectory)/widgets_scrollarea$(ObjectSuffix) $(IntermediateDirectory)/widgets_slider$(ObjectSuffix) $(IntermediateDirectory)/widgets_tab$(ObjectSuffix) $(IntermediateDirectory)/widgets_tabbedarea$(ObjectSuffix) $(IntermediateDirectory)/widgets_textbox$(ObjectSuffix) $(IntermediateDirectory)/widgets_textfield$(ObjectSuffix) $(IntermediateDirectory)/widgets_window$(ObjectSuffix) \
	$(IntermediateDirectory)/allegro_allegro$(ObjectSuffix) $(IntermediateDirectory)/allegro_allegrofont$(ObjectSuffix) $(IntermediateDirectory)/allegro_allegrographics$(ObjectSuffix) $(IntermediateDirectory)/allegro_allegroimage$(ObjectSuffix) $(IntermediateDirectory)/allegro_allegroimageloader$(ObjectSuffix) $(IntermediateDirectory)/allegro_allegroinput$(ObjectSuffix) 



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
	@$(MakeDirCommand) "D:\Stuff\CODE\CodeLite\Stas1/.build-release"
	@echo rebuilt > "D:\Stuff\CODE\CodeLite\Stas1/.build-release/guichan"

./Release:
	@$(MakeDirCommand) "./Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_actionevent$(ObjectSuffix): src/actionevent.cpp $(IntermediateDirectory)/src_actionevent$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/actionevent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_actionevent$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_actionevent$(DependSuffix): src/actionevent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_actionevent$(ObjectSuffix) -MF$(IntermediateDirectory)/src_actionevent$(DependSuffix) -MM "src/actionevent.cpp"

$(IntermediateDirectory)/src_actionevent$(PreprocessSuffix): src/actionevent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_actionevent$(PreprocessSuffix) "src/actionevent.cpp"

$(IntermediateDirectory)/src_basiccontainer$(ObjectSuffix): src/basiccontainer.cpp $(IntermediateDirectory)/src_basiccontainer$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/basiccontainer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_basiccontainer$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_basiccontainer$(DependSuffix): src/basiccontainer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_basiccontainer$(ObjectSuffix) -MF$(IntermediateDirectory)/src_basiccontainer$(DependSuffix) -MM "src/basiccontainer.cpp"

$(IntermediateDirectory)/src_basiccontainer$(PreprocessSuffix): src/basiccontainer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_basiccontainer$(PreprocessSuffix) "src/basiccontainer.cpp"

$(IntermediateDirectory)/src_cliprectangle$(ObjectSuffix): src/cliprectangle.cpp $(IntermediateDirectory)/src_cliprectangle$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/cliprectangle.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_cliprectangle$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_cliprectangle$(DependSuffix): src/cliprectangle.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_cliprectangle$(ObjectSuffix) -MF$(IntermediateDirectory)/src_cliprectangle$(DependSuffix) -MM "src/cliprectangle.cpp"

$(IntermediateDirectory)/src_cliprectangle$(PreprocessSuffix): src/cliprectangle.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_cliprectangle$(PreprocessSuffix) "src/cliprectangle.cpp"

$(IntermediateDirectory)/src_color$(ObjectSuffix): src/color.cpp $(IntermediateDirectory)/src_color$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/color.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_color$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_color$(DependSuffix): src/color.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_color$(ObjectSuffix) -MF$(IntermediateDirectory)/src_color$(DependSuffix) -MM "src/color.cpp"

$(IntermediateDirectory)/src_color$(PreprocessSuffix): src/color.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_color$(PreprocessSuffix) "src/color.cpp"

$(IntermediateDirectory)/src_defaultfont$(ObjectSuffix): src/defaultfont.cpp $(IntermediateDirectory)/src_defaultfont$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/defaultfont.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_defaultfont$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_defaultfont$(DependSuffix): src/defaultfont.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_defaultfont$(ObjectSuffix) -MF$(IntermediateDirectory)/src_defaultfont$(DependSuffix) -MM "src/defaultfont.cpp"

$(IntermediateDirectory)/src_defaultfont$(PreprocessSuffix): src/defaultfont.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_defaultfont$(PreprocessSuffix) "src/defaultfont.cpp"

$(IntermediateDirectory)/src_event$(ObjectSuffix): src/event.cpp $(IntermediateDirectory)/src_event$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/event.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_event$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_event$(DependSuffix): src/event.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_event$(ObjectSuffix) -MF$(IntermediateDirectory)/src_event$(DependSuffix) -MM "src/event.cpp"

$(IntermediateDirectory)/src_event$(PreprocessSuffix): src/event.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_event$(PreprocessSuffix) "src/event.cpp"

$(IntermediateDirectory)/src_exception$(ObjectSuffix): src/exception.cpp $(IntermediateDirectory)/src_exception$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/exception.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_exception$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_exception$(DependSuffix): src/exception.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_exception$(ObjectSuffix) -MF$(IntermediateDirectory)/src_exception$(DependSuffix) -MM "src/exception.cpp"

$(IntermediateDirectory)/src_exception$(PreprocessSuffix): src/exception.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_exception$(PreprocessSuffix) "src/exception.cpp"

$(IntermediateDirectory)/src_focushandler$(ObjectSuffix): src/focushandler.cpp $(IntermediateDirectory)/src_focushandler$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/focushandler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_focushandler$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_focushandler$(DependSuffix): src/focushandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_focushandler$(ObjectSuffix) -MF$(IntermediateDirectory)/src_focushandler$(DependSuffix) -MM "src/focushandler.cpp"

$(IntermediateDirectory)/src_focushandler$(PreprocessSuffix): src/focushandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_focushandler$(PreprocessSuffix) "src/focushandler.cpp"

$(IntermediateDirectory)/src_font$(ObjectSuffix): src/font.cpp $(IntermediateDirectory)/src_font$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/font.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_font$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_font$(DependSuffix): src/font.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_font$(ObjectSuffix) -MF$(IntermediateDirectory)/src_font$(DependSuffix) -MM "src/font.cpp"

$(IntermediateDirectory)/src_font$(PreprocessSuffix): src/font.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_font$(PreprocessSuffix) "src/font.cpp"

$(IntermediateDirectory)/src_genericinput$(ObjectSuffix): src/genericinput.cpp $(IntermediateDirectory)/src_genericinput$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/genericinput.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_genericinput$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_genericinput$(DependSuffix): src/genericinput.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_genericinput$(ObjectSuffix) -MF$(IntermediateDirectory)/src_genericinput$(DependSuffix) -MM "src/genericinput.cpp"

$(IntermediateDirectory)/src_genericinput$(PreprocessSuffix): src/genericinput.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_genericinput$(PreprocessSuffix) "src/genericinput.cpp"

$(IntermediateDirectory)/src_graphics$(ObjectSuffix): src/graphics.cpp $(IntermediateDirectory)/src_graphics$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/graphics.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_graphics$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_graphics$(DependSuffix): src/graphics.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_graphics$(ObjectSuffix) -MF$(IntermediateDirectory)/src_graphics$(DependSuffix) -MM "src/graphics.cpp"

$(IntermediateDirectory)/src_graphics$(PreprocessSuffix): src/graphics.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_graphics$(PreprocessSuffix) "src/graphics.cpp"

$(IntermediateDirectory)/src_gui$(ObjectSuffix): src/gui.cpp $(IntermediateDirectory)/src_gui$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/gui.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gui$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gui$(DependSuffix): src/gui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gui$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gui$(DependSuffix) -MM "src/gui.cpp"

$(IntermediateDirectory)/src_gui$(PreprocessSuffix): src/gui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gui$(PreprocessSuffix) "src/gui.cpp"

$(IntermediateDirectory)/src_guichan$(ObjectSuffix): src/guichan.cpp $(IntermediateDirectory)/src_guichan$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/guichan.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_guichan$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_guichan$(DependSuffix): src/guichan.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_guichan$(ObjectSuffix) -MF$(IntermediateDirectory)/src_guichan$(DependSuffix) -MM "src/guichan.cpp"

$(IntermediateDirectory)/src_guichan$(PreprocessSuffix): src/guichan.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_guichan$(PreprocessSuffix) "src/guichan.cpp"

$(IntermediateDirectory)/src_image$(ObjectSuffix): src/image.cpp $(IntermediateDirectory)/src_image$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/image.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_image$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_image$(DependSuffix): src/image.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_image$(ObjectSuffix) -MF$(IntermediateDirectory)/src_image$(DependSuffix) -MM "src/image.cpp"

$(IntermediateDirectory)/src_image$(PreprocessSuffix): src/image.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_image$(PreprocessSuffix) "src/image.cpp"

$(IntermediateDirectory)/src_imagefont$(ObjectSuffix): src/imagefont.cpp $(IntermediateDirectory)/src_imagefont$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/imagefont.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_imagefont$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_imagefont$(DependSuffix): src/imagefont.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_imagefont$(ObjectSuffix) -MF$(IntermediateDirectory)/src_imagefont$(DependSuffix) -MM "src/imagefont.cpp"

$(IntermediateDirectory)/src_imagefont$(PreprocessSuffix): src/imagefont.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_imagefont$(PreprocessSuffix) "src/imagefont.cpp"

$(IntermediateDirectory)/src_inputevent$(ObjectSuffix): src/inputevent.cpp $(IntermediateDirectory)/src_inputevent$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/inputevent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_inputevent$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_inputevent$(DependSuffix): src/inputevent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_inputevent$(ObjectSuffix) -MF$(IntermediateDirectory)/src_inputevent$(DependSuffix) -MM "src/inputevent.cpp"

$(IntermediateDirectory)/src_inputevent$(PreprocessSuffix): src/inputevent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_inputevent$(PreprocessSuffix) "src/inputevent.cpp"

$(IntermediateDirectory)/src_key$(ObjectSuffix): src/key.cpp $(IntermediateDirectory)/src_key$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/key.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_key$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_key$(DependSuffix): src/key.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_key$(ObjectSuffix) -MF$(IntermediateDirectory)/src_key$(DependSuffix) -MM "src/key.cpp"

$(IntermediateDirectory)/src_key$(PreprocessSuffix): src/key.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_key$(PreprocessSuffix) "src/key.cpp"

$(IntermediateDirectory)/src_keyevent$(ObjectSuffix): src/keyevent.cpp $(IntermediateDirectory)/src_keyevent$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/keyevent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_keyevent$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_keyevent$(DependSuffix): src/keyevent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_keyevent$(ObjectSuffix) -MF$(IntermediateDirectory)/src_keyevent$(DependSuffix) -MM "src/keyevent.cpp"

$(IntermediateDirectory)/src_keyevent$(PreprocessSuffix): src/keyevent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_keyevent$(PreprocessSuffix) "src/keyevent.cpp"

$(IntermediateDirectory)/src_keyinput$(ObjectSuffix): src/keyinput.cpp $(IntermediateDirectory)/src_keyinput$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/keyinput.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_keyinput$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_keyinput$(DependSuffix): src/keyinput.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_keyinput$(ObjectSuffix) -MF$(IntermediateDirectory)/src_keyinput$(DependSuffix) -MM "src/keyinput.cpp"

$(IntermediateDirectory)/src_keyinput$(PreprocessSuffix): src/keyinput.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_keyinput$(PreprocessSuffix) "src/keyinput.cpp"

$(IntermediateDirectory)/src_mouseevent$(ObjectSuffix): src/mouseevent.cpp $(IntermediateDirectory)/src_mouseevent$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/mouseevent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_mouseevent$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_mouseevent$(DependSuffix): src/mouseevent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_mouseevent$(ObjectSuffix) -MF$(IntermediateDirectory)/src_mouseevent$(DependSuffix) -MM "src/mouseevent.cpp"

$(IntermediateDirectory)/src_mouseevent$(PreprocessSuffix): src/mouseevent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_mouseevent$(PreprocessSuffix) "src/mouseevent.cpp"

$(IntermediateDirectory)/src_mouseinput$(ObjectSuffix): src/mouseinput.cpp $(IntermediateDirectory)/src_mouseinput$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/mouseinput.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_mouseinput$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_mouseinput$(DependSuffix): src/mouseinput.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_mouseinput$(ObjectSuffix) -MF$(IntermediateDirectory)/src_mouseinput$(DependSuffix) -MM "src/mouseinput.cpp"

$(IntermediateDirectory)/src_mouseinput$(PreprocessSuffix): src/mouseinput.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_mouseinput$(PreprocessSuffix) "src/mouseinput.cpp"

$(IntermediateDirectory)/src_rectangle$(ObjectSuffix): src/rectangle.cpp $(IntermediateDirectory)/src_rectangle$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/rectangle.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_rectangle$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_rectangle$(DependSuffix): src/rectangle.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_rectangle$(ObjectSuffix) -MF$(IntermediateDirectory)/src_rectangle$(DependSuffix) -MM "src/rectangle.cpp"

$(IntermediateDirectory)/src_rectangle$(PreprocessSuffix): src/rectangle.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_rectangle$(PreprocessSuffix) "src/rectangle.cpp"

$(IntermediateDirectory)/src_selectionevent$(ObjectSuffix): src/selectionevent.cpp $(IntermediateDirectory)/src_selectionevent$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/selectionevent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_selectionevent$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_selectionevent$(DependSuffix): src/selectionevent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_selectionevent$(ObjectSuffix) -MF$(IntermediateDirectory)/src_selectionevent$(DependSuffix) -MM "src/selectionevent.cpp"

$(IntermediateDirectory)/src_selectionevent$(PreprocessSuffix): src/selectionevent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_selectionevent$(PreprocessSuffix) "src/selectionevent.cpp"

$(IntermediateDirectory)/src_widget$(ObjectSuffix): src/widget.cpp $(IntermediateDirectory)/src_widget$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widget.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_widget$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_widget$(DependSuffix): src/widget.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_widget$(ObjectSuffix) -MF$(IntermediateDirectory)/src_widget$(DependSuffix) -MM "src/widget.cpp"

$(IntermediateDirectory)/src_widget$(PreprocessSuffix): src/widget.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_widget$(PreprocessSuffix) "src/widget.cpp"

$(IntermediateDirectory)/widgets_button$(ObjectSuffix): src/widgets/button.cpp $(IntermediateDirectory)/widgets_button$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/button.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_button$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_button$(DependSuffix): src/widgets/button.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_button$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_button$(DependSuffix) -MM "src/widgets/button.cpp"

$(IntermediateDirectory)/widgets_button$(PreprocessSuffix): src/widgets/button.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_button$(PreprocessSuffix) "src/widgets/button.cpp"

$(IntermediateDirectory)/widgets_checkbox$(ObjectSuffix): src/widgets/checkbox.cpp $(IntermediateDirectory)/widgets_checkbox$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/checkbox.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_checkbox$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_checkbox$(DependSuffix): src/widgets/checkbox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_checkbox$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_checkbox$(DependSuffix) -MM "src/widgets/checkbox.cpp"

$(IntermediateDirectory)/widgets_checkbox$(PreprocessSuffix): src/widgets/checkbox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_checkbox$(PreprocessSuffix) "src/widgets/checkbox.cpp"

$(IntermediateDirectory)/widgets_container$(ObjectSuffix): src/widgets/container.cpp $(IntermediateDirectory)/widgets_container$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/container.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_container$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_container$(DependSuffix): src/widgets/container.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_container$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_container$(DependSuffix) -MM "src/widgets/container.cpp"

$(IntermediateDirectory)/widgets_container$(PreprocessSuffix): src/widgets/container.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_container$(PreprocessSuffix) "src/widgets/container.cpp"

$(IntermediateDirectory)/widgets_dropdown$(ObjectSuffix): src/widgets/dropdown.cpp $(IntermediateDirectory)/widgets_dropdown$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/dropdown.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_dropdown$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_dropdown$(DependSuffix): src/widgets/dropdown.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_dropdown$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_dropdown$(DependSuffix) -MM "src/widgets/dropdown.cpp"

$(IntermediateDirectory)/widgets_dropdown$(PreprocessSuffix): src/widgets/dropdown.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_dropdown$(PreprocessSuffix) "src/widgets/dropdown.cpp"

$(IntermediateDirectory)/widgets_icon$(ObjectSuffix): src/widgets/icon.cpp $(IntermediateDirectory)/widgets_icon$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/icon.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_icon$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_icon$(DependSuffix): src/widgets/icon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_icon$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_icon$(DependSuffix) -MM "src/widgets/icon.cpp"

$(IntermediateDirectory)/widgets_icon$(PreprocessSuffix): src/widgets/icon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_icon$(PreprocessSuffix) "src/widgets/icon.cpp"

$(IntermediateDirectory)/widgets_imagebutton$(ObjectSuffix): src/widgets/imagebutton.cpp $(IntermediateDirectory)/widgets_imagebutton$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/imagebutton.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_imagebutton$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_imagebutton$(DependSuffix): src/widgets/imagebutton.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_imagebutton$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_imagebutton$(DependSuffix) -MM "src/widgets/imagebutton.cpp"

$(IntermediateDirectory)/widgets_imagebutton$(PreprocessSuffix): src/widgets/imagebutton.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_imagebutton$(PreprocessSuffix) "src/widgets/imagebutton.cpp"

$(IntermediateDirectory)/widgets_label$(ObjectSuffix): src/widgets/label.cpp $(IntermediateDirectory)/widgets_label$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/label.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_label$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_label$(DependSuffix): src/widgets/label.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_label$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_label$(DependSuffix) -MM "src/widgets/label.cpp"

$(IntermediateDirectory)/widgets_label$(PreprocessSuffix): src/widgets/label.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_label$(PreprocessSuffix) "src/widgets/label.cpp"

$(IntermediateDirectory)/widgets_listbox$(ObjectSuffix): src/widgets/listbox.cpp $(IntermediateDirectory)/widgets_listbox$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/listbox.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_listbox$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_listbox$(DependSuffix): src/widgets/listbox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_listbox$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_listbox$(DependSuffix) -MM "src/widgets/listbox.cpp"

$(IntermediateDirectory)/widgets_listbox$(PreprocessSuffix): src/widgets/listbox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_listbox$(PreprocessSuffix) "src/widgets/listbox.cpp"

$(IntermediateDirectory)/widgets_radiobutton$(ObjectSuffix): src/widgets/radiobutton.cpp $(IntermediateDirectory)/widgets_radiobutton$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/radiobutton.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_radiobutton$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_radiobutton$(DependSuffix): src/widgets/radiobutton.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_radiobutton$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_radiobutton$(DependSuffix) -MM "src/widgets/radiobutton.cpp"

$(IntermediateDirectory)/widgets_radiobutton$(PreprocessSuffix): src/widgets/radiobutton.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_radiobutton$(PreprocessSuffix) "src/widgets/radiobutton.cpp"

$(IntermediateDirectory)/widgets_scrollarea$(ObjectSuffix): src/widgets/scrollarea.cpp $(IntermediateDirectory)/widgets_scrollarea$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/scrollarea.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_scrollarea$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_scrollarea$(DependSuffix): src/widgets/scrollarea.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_scrollarea$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_scrollarea$(DependSuffix) -MM "src/widgets/scrollarea.cpp"

$(IntermediateDirectory)/widgets_scrollarea$(PreprocessSuffix): src/widgets/scrollarea.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_scrollarea$(PreprocessSuffix) "src/widgets/scrollarea.cpp"

$(IntermediateDirectory)/widgets_slider$(ObjectSuffix): src/widgets/slider.cpp $(IntermediateDirectory)/widgets_slider$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/slider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_slider$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_slider$(DependSuffix): src/widgets/slider.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_slider$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_slider$(DependSuffix) -MM "src/widgets/slider.cpp"

$(IntermediateDirectory)/widgets_slider$(PreprocessSuffix): src/widgets/slider.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_slider$(PreprocessSuffix) "src/widgets/slider.cpp"

$(IntermediateDirectory)/widgets_tab$(ObjectSuffix): src/widgets/tab.cpp $(IntermediateDirectory)/widgets_tab$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/tab.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_tab$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_tab$(DependSuffix): src/widgets/tab.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_tab$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_tab$(DependSuffix) -MM "src/widgets/tab.cpp"

$(IntermediateDirectory)/widgets_tab$(PreprocessSuffix): src/widgets/tab.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_tab$(PreprocessSuffix) "src/widgets/tab.cpp"

$(IntermediateDirectory)/widgets_tabbedarea$(ObjectSuffix): src/widgets/tabbedarea.cpp $(IntermediateDirectory)/widgets_tabbedarea$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/tabbedarea.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_tabbedarea$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_tabbedarea$(DependSuffix): src/widgets/tabbedarea.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_tabbedarea$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_tabbedarea$(DependSuffix) -MM "src/widgets/tabbedarea.cpp"

$(IntermediateDirectory)/widgets_tabbedarea$(PreprocessSuffix): src/widgets/tabbedarea.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_tabbedarea$(PreprocessSuffix) "src/widgets/tabbedarea.cpp"

$(IntermediateDirectory)/widgets_textbox$(ObjectSuffix): src/widgets/textbox.cpp $(IntermediateDirectory)/widgets_textbox$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/textbox.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_textbox$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_textbox$(DependSuffix): src/widgets/textbox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_textbox$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_textbox$(DependSuffix) -MM "src/widgets/textbox.cpp"

$(IntermediateDirectory)/widgets_textbox$(PreprocessSuffix): src/widgets/textbox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_textbox$(PreprocessSuffix) "src/widgets/textbox.cpp"

$(IntermediateDirectory)/widgets_textfield$(ObjectSuffix): src/widgets/textfield.cpp $(IntermediateDirectory)/widgets_textfield$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/textfield.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_textfield$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_textfield$(DependSuffix): src/widgets/textfield.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_textfield$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_textfield$(DependSuffix) -MM "src/widgets/textfield.cpp"

$(IntermediateDirectory)/widgets_textfield$(PreprocessSuffix): src/widgets/textfield.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_textfield$(PreprocessSuffix) "src/widgets/textfield.cpp"

$(IntermediateDirectory)/widgets_window$(ObjectSuffix): src/widgets/window.cpp $(IntermediateDirectory)/widgets_window$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/widgets/window.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/widgets_window$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/widgets_window$(DependSuffix): src/widgets/window.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/widgets_window$(ObjectSuffix) -MF$(IntermediateDirectory)/widgets_window$(DependSuffix) -MM "src/widgets/window.cpp"

$(IntermediateDirectory)/widgets_window$(PreprocessSuffix): src/widgets/window.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/widgets_window$(PreprocessSuffix) "src/widgets/window.cpp"

$(IntermediateDirectory)/allegro_allegro$(ObjectSuffix): src/allegro/allegro.cpp $(IntermediateDirectory)/allegro_allegro$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/allegro/allegro.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/allegro_allegro$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/allegro_allegro$(DependSuffix): src/allegro/allegro.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/allegro_allegro$(ObjectSuffix) -MF$(IntermediateDirectory)/allegro_allegro$(DependSuffix) -MM "src/allegro/allegro.cpp"

$(IntermediateDirectory)/allegro_allegro$(PreprocessSuffix): src/allegro/allegro.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/allegro_allegro$(PreprocessSuffix) "src/allegro/allegro.cpp"

$(IntermediateDirectory)/allegro_allegrofont$(ObjectSuffix): src/allegro/allegrofont.cpp $(IntermediateDirectory)/allegro_allegrofont$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/allegro/allegrofont.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/allegro_allegrofont$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/allegro_allegrofont$(DependSuffix): src/allegro/allegrofont.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/allegro_allegrofont$(ObjectSuffix) -MF$(IntermediateDirectory)/allegro_allegrofont$(DependSuffix) -MM "src/allegro/allegrofont.cpp"

$(IntermediateDirectory)/allegro_allegrofont$(PreprocessSuffix): src/allegro/allegrofont.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/allegro_allegrofont$(PreprocessSuffix) "src/allegro/allegrofont.cpp"

$(IntermediateDirectory)/allegro_allegrographics$(ObjectSuffix): src/allegro/allegrographics.cpp $(IntermediateDirectory)/allegro_allegrographics$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/allegro/allegrographics.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/allegro_allegrographics$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/allegro_allegrographics$(DependSuffix): src/allegro/allegrographics.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/allegro_allegrographics$(ObjectSuffix) -MF$(IntermediateDirectory)/allegro_allegrographics$(DependSuffix) -MM "src/allegro/allegrographics.cpp"

$(IntermediateDirectory)/allegro_allegrographics$(PreprocessSuffix): src/allegro/allegrographics.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/allegro_allegrographics$(PreprocessSuffix) "src/allegro/allegrographics.cpp"

$(IntermediateDirectory)/allegro_allegroimage$(ObjectSuffix): src/allegro/allegroimage.cpp $(IntermediateDirectory)/allegro_allegroimage$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/allegro/allegroimage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/allegro_allegroimage$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/allegro_allegroimage$(DependSuffix): src/allegro/allegroimage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/allegro_allegroimage$(ObjectSuffix) -MF$(IntermediateDirectory)/allegro_allegroimage$(DependSuffix) -MM "src/allegro/allegroimage.cpp"

$(IntermediateDirectory)/allegro_allegroimage$(PreprocessSuffix): src/allegro/allegroimage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/allegro_allegroimage$(PreprocessSuffix) "src/allegro/allegroimage.cpp"

$(IntermediateDirectory)/allegro_allegroimageloader$(ObjectSuffix): src/allegro/allegroimageloader.cpp $(IntermediateDirectory)/allegro_allegroimageloader$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/allegro/allegroimageloader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/allegro_allegroimageloader$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/allegro_allegroimageloader$(DependSuffix): src/allegro/allegroimageloader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/allegro_allegroimageloader$(ObjectSuffix) -MF$(IntermediateDirectory)/allegro_allegroimageloader$(DependSuffix) -MM "src/allegro/allegroimageloader.cpp"

$(IntermediateDirectory)/allegro_allegroimageloader$(PreprocessSuffix): src/allegro/allegroimageloader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/allegro_allegroimageloader$(PreprocessSuffix) "src/allegro/allegroimageloader.cpp"

$(IntermediateDirectory)/allegro_allegroinput$(ObjectSuffix): src/allegro/allegroinput.cpp $(IntermediateDirectory)/allegro_allegroinput$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "D:/Stuff/CODE/CodeLite/Stas1/guichan/src/allegro/allegroinput.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/allegro_allegroinput$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/allegro_allegroinput$(DependSuffix): src/allegro/allegroinput.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/allegro_allegroinput$(ObjectSuffix) -MF$(IntermediateDirectory)/allegro_allegroinput$(DependSuffix) -MM "src/allegro/allegroinput.cpp"

$(IntermediateDirectory)/allegro_allegroinput$(PreprocessSuffix): src/allegro/allegroinput.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/allegro_allegroinput$(PreprocessSuffix) "src/allegro/allegroinput.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/src_actionevent$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_actionevent$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_actionevent$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_basiccontainer$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_basiccontainer$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_basiccontainer$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_cliprectangle$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_cliprectangle$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_cliprectangle$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_color$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_color$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_color$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_defaultfont$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_defaultfont$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_defaultfont$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_event$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_event$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_event$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_exception$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_exception$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_exception$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_focushandler$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_focushandler$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_focushandler$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_font$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_font$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_font$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_genericinput$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_genericinput$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_genericinput$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_graphics$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_graphics$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_graphics$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_gui$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_gui$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_gui$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_guichan$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_guichan$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_guichan$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_image$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_image$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_image$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_imagefont$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_imagefont$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_imagefont$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_inputevent$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_inputevent$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_inputevent$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_key$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_key$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_key$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_keyevent$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_keyevent$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_keyevent$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_keyinput$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_keyinput$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_keyinput$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_mouseevent$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_mouseevent$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_mouseevent$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_mouseinput$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_mouseinput$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_mouseinput$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_rectangle$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_rectangle$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_rectangle$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_selectionevent$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_selectionevent$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_selectionevent$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_widget$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_widget$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_widget$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_button$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_button$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_button$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_checkbox$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_checkbox$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_checkbox$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_container$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_container$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_container$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_dropdown$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_dropdown$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_dropdown$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_icon$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_icon$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_icon$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_imagebutton$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_imagebutton$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_imagebutton$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_label$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_label$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_label$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_listbox$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_listbox$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_listbox$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_radiobutton$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_radiobutton$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_radiobutton$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_scrollarea$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_scrollarea$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_scrollarea$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_slider$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_slider$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_slider$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_tab$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_tab$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_tab$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_tabbedarea$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_tabbedarea$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_tabbedarea$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_textbox$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_textbox$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_textbox$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_textfield$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_textfield$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_textfield$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/widgets_window$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/widgets_window$(DependSuffix)
	$(RM) $(IntermediateDirectory)/widgets_window$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegro$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegro$(DependSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegro$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegrofont$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegrofont$(DependSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegrofont$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegrographics$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegrographics$(DependSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegrographics$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegroimage$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegroimage$(DependSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegroimage$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegroimageloader$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegroimageloader$(DependSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegroimageloader$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegroinput$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegroinput$(DependSuffix)
	$(RM) $(IntermediateDirectory)/allegro_allegroinput$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile)
	$(RM) "../.build-release/guichan"


