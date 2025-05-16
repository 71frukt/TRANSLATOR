USE_DEBUG ?= true

RES_ASM_NAME = ../../res_asm.asm 		# the path relative to the compiler folder
SUBPROJECTS  = frontend middlend backend tree tree_saver compiler spu

# *proj_name in capital letters*_DIR = ***
FRONTEND_DIR   = frontend
MIDDLEND_DIR   = middlend
BACKEND_DIR    = backend
TREE_DIR 	   = tree
TREE_SAVER_DIR = tree_saver
COMPILER_DIR   = SPU/compiler
SPU_DIR 	   = SPU/spu



#---------------------------------------------------------------------------------------------
.PHONY: all clean frontend middlend backend build_exe run_exe  $(foreach proj, $(SUBPROJECTS), $(proj) clean_$(proj) rebuild_$(proj))

all: $(SUBPROJECTS)

clean_all:   $(addprefix clean_,   $(SUBPROJECTS))

rebuild_all: $(addprefix rebuild_, $(SUBPROJECTS))

build_exe: run_frontend run_backend spu_compile

spu_compile:
	$(MAKE) run RES_ASM_NAME=$(RES_ASM_NAME) -C $(COMPILER_DIR)

run_exe:
	$(MAKE) run -C $(SPU_DIR)


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




# all: frontend backend tree tree_saver

# clean_all:
# 	@echo "cleaning frontend, middlend and backend"
# 	$(MAKE) -C frontend clean
# 	$(MAKE) -C middlend clean
# 	$(MAKE) -C backend  clean



# frontend:
# 	@echo "building frontend"
# 	$(MAKE) -C $(FRONTEND_DIR)

# clean_frontend:
# 	@echo "cleaning frontend"
# 	$(MAKE) clean -C $(FRONTEND_DIR)

# rebuild_frontend:
# 	@echo "rebuilding frontend"
# 	$(MAKE) rebuild -C $(FRONTEND_DIR)


# middlend:
# 	@echo "building middlend"
# 	$(MAKE) -C $(MIDDLEND_DIR)

# clean_middlend:
# 	@echo "cleaning middlend"
# 	$(MAKE) clean -C $(MIDDLEND_DIR)

# rebuild_middlend:
# 	@echo "rebuilding middlend"
# 	$(MAKE) rebuild -C $(MIDDLEND_DIR)


# backend:
# 	@echo "building backend"
# 	$(MAKE) -C $(BACKEND_DIR)

# clean_backend:
# 	@echo "cleaning backend"
# 	$(MAKE) clean -C $(BACKEND_DIR)

# rebuild_backend:
# 	@echo "rebuilding backend"
# 	$(MAKE) rebuild -C $(BACKEND_DIR)


# tree:
# 	@echo "building tree"
# 	$(MAKE) -C $(FRONTEND_DIR)

# clean_tree:
# 	@echo "cleaning tree"
# 	$(MAKE) clean -C $(FRONTEND_DIR)

# rebuild_tree:
# 	@echo "rebuilding tree"
# 	$(MAKE) rebuild -C $(FRONTEND_DIR)