FLASHTOOL_TARGET_x32 	= cp1625_flash_bootloader32
FLASHTOOL_TARGET_x64 	= cp1625_flash_bootloader64
TESTTOOL_TARGET		= cp1625_flash_bootloader
SOURCE	   			= tools/cp1625_flash_bootloader/src
SOURCE_COMMON			= common
OBJDIR 					= ./_build
OBJDIRx32 				= $(OBJDIR)/x32
OBJDIRx64 				= $(OBJDIR)/x64

OBJS_FlashTool = \
	$(SOURCE)/Main.obj \
	$(SOURCE)/cp1625_flash_bootloader.obj \
	$(SOURCE_COMMON)/os_Env.obj \
	$(SOURCE_COMMON)/PnCore.obj \
	$(SOURCE_COMMON)/PnDev_DriverU.obj \
	$(SOURCE_COMMON)/PnDev_Util.obj 
		
		
INCD = \
	-I../../$(SOURCE) \
	-I../../$(SOURCE_COMMON) \
	-I../../$(SOURCE)/../../../lsa \
	-I../../$(SOURCE)/../../../../../pnio/src/common \
		
		
DEFD = \
	   -DPNDEV_OS_LINUX \
	   -D_GNU_SOURCE
	   

all: $(FLASHTOOL_TARGET_x32) $(FLASHTOOL_TARGET_x64)
	@echo ALL done for 'cp1625_flash_bootloader'

clean:
	rm -f $(FLASHTOOL_TARGET_x32) $(FLASHTOOL_TARGET_x64)
	rm -r $(OBJDIR)
	@echo Clean All done

$(FLASHTOOL_TARGET_x32) : CCFLAGS += -m32 -MMD -pthread -Wfatal-errors 
$(FLASHTOOL_TARGET_x32) : $(addprefix $(OBJDIRx32)/,$(addsuffix .o,$(basename $(OBJS_FlashTool))))
	$(CXX) $(DEFD) $(CCFLAGS) $(INCD) -g -o $@ $^ -ldl -lrt
	@echo Building $@ done

	
$(FLASHTOOL_TARGET_x64) : CCFLAGS = -MMD -pthread -Wfatal-errors
$(FLASHTOOL_TARGET_x64) : $(addprefix $(OBJDIRx64)/,$(addsuffix .o,$(basename $(OBJS_FlashTool))))
	$(CXX) $(DEFD) $(CCFLAGS) $(INCD) -g -o $@ $^ -ldl -lrt
	@echo Building $@ done
	
-include $(addprefix $(OBJDIRx32)/,$(addsuffix .d,$(basename $(OBJS_FlashTool))))
-include $(addprefix $(OBJDIRx64)/,$(addsuffix .d,$(basename $(OBJS_FlashTool))))

	
$(OBJDIRx32)/%.o : ../../%.cpp
	@mkdir -p $(@D)
	$(CXX) $(DEFD) $(CCFLAGS) $(INCD) -g -o $@ -c $<

$(OBJDIRx32)/%.o : ../../%.c
	@mkdir -p $(@D)
	$(CC) $(DEFD) $(CCFLAGS) $(INCD) -g -o $@ -c $<

	
$(OBJDIRx64)/%.o : ../../%.cpp
	@mkdir -p $(@D)
	$(CXX) $(DEFD) $(CCFLAGS) $(INCD) -g -o $@ -c $<

$(OBJDIRx64)/%.o : ../../%.c
	@mkdir -p $(@D)
	$(CC) $(DEFD) $(CCFLAGS) $(INCD) -g -o $@ -c $<

