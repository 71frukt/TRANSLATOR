USE_DEBUG ?= true

RES_SPU_ASM_NAME = ../../res_asm.asm 		# the path relative to the compiler folder
# RES_TRANSLATOR_ASM_NAME = translator.asm

SUBPROJECTS  = tree tree_saver compiler spu logger ir_builder ir_backend elf_builder frontend middlend backend

# *proj_name in capital letters*_DIR = ***
FRONTEND_DIR    = frontend
MIDDLEND_DIR    = middlend
BACKEND_DIR     = backend
TREE_DIR 	    = tree
TREE_SAVER_DIR  = tree_saver
COMPILER_DIR    = SPU/compiler
SPU_DIR 	    = SPU/spu
LOGGER_DIR		= logger
IR_BUILDER_DIR	= ir_builder
IR_BACKEND_DIR  = ir_backend
ELF_BUILDER_DIR = elf_builder



#---------------------------------------------------------------------------------------------
.PHONY: all clean frontend middlend backend build_spu build_asm run_translator $(foreach proj, $(SUBPROJECTS), $(proj) clean_$(proj) rebuild_$(proj))

all: $(SUBPROJECTS)

clean_all:   $(addprefix clean_,   $(SUBPROJECTS))

rebuild_all: $(addprefix rebuild_, $(SUBPROJECTS))

build_spu: run_frontend run_backend spu_compile

spu_compile:
	$(MAKE) run RES_SPU_ASM_NAME=$(RES_SPU_ASM_NAME) -C $(COMPILER_DIR)


$(foreach proj, $(SUBPROJECTS), 														   									\
	$(eval $(proj): 												  	  				   								  ; \
		@echo -e "\nbuilding $(proj)"                                      	  				   							  ; \
		$$(MAKE) EXECUTABLE=$(proj) USE_DEBUG=$(USE_DEBUG) -C $$($$(shell echo $(proj) | tr 'a-z' 'A-Z')_DIR)     			\
	) 																														\
																															\
	$(eval clean_$(proj): 												  												  ; \
		@echo -e "\ncleaning $(proj)"										  		  			  						  ; \
		$$(MAKE) clean -C $$($$(shell echo $(proj) | tr 'a-z' 'A-Z')_DIR)   												\
	) 																														\
																															\
	$(eval rebuild_$(proj): 											  						 						  ; \
		@echo -e "\nrebuilding $(proj)"										  					 						  ; \
		$$(MAKE) EXECUTABLE=$(proj) USE_DEBUG=$(USE_DEBUG) rebuild -C $$($$(shell echo $(proj) | tr 'a-z' 'A-Z')_DIR) 		\
	) 																														\
																															\
	$(eval run_$(proj): 												  	  				   							  ; \
		@echo -e "\nbuilding $(proj)"                                      	  				   							  ; \
		$$(MAKE) EXECUTABLE=$(proj) USE_DEBUG=$(USE_DEBUG) run -C $$($$(shell echo $(proj) | tr 'a-z' 'A-Z')_DIR)     		\
	)																														\
)


run_translator: run_frontend run_middlend run_elf_builder

build_asm:
	$(MAKE) -f makeasm run

benchmark_translator:
	python tr_benchmark/perf_profile_p_cores.py

benchmark_spu:
	cd SPU	&& \
	cd spu	&& \
	python perf_profile_p_cores.py	&& \
	cd ..	&& \
	cd ..