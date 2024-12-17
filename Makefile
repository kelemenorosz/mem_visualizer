
SOURCE_DIR = source
BUILD_DIR = build

!IF !EXIST("$(BUILD_DIR)")
!	IF [mkdir $(BUILD_DIR)] != 0
!	ENDIF
!ENDIF

all: main.exe
	@echo ALL DONE

main.exe: $(BUILD_DIR)/*.obj
	@link $** /SUBSYSTEM:WINDOWS /OUT:main.exe User32.lib Gdi32.lib Opengl32.lib
	@echo LINKING DONE

{$(SOURCE_DIR)}.cpp{$(BUILD_DIR)}.obj:
	@cl /c /I$(SOURCE_DIR)/ $< /Fo$(BUILD_DIR)/
	@echo BUILDING DONE