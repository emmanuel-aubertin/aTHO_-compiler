all:	test
clean:
	@echo "\n\e[32m--------| \e[1;32mCLEANING\e[0m\e[32m |--------\e[0m"
	rm -f *.o *.s
	rm -f tokeniser.cpp test
	@echo "\e[32m\tDONE\e[0m"
tokeniser.cpp:	tokeniser.l
	@echo "\n\e[32m--------| \e[1;32mCompilation of tokeniser.l\e[0m\e[32m |--------\e[0m"
	flex++ -d -otokeniser.cpp tokeniser.l
	@echo "\e[32m\tDONE\e[0m\n"
tokeniser.o:	tokeniser.cpp
	@echo "\e[32m--------| \e[1;32mCompilation of tokeniser.cpp\e[0m\e[32m |--------\e[0m"
	g++ -c tokeniser.cpp
	@echo "\e[32m\tDONE\e[0m\n"
compilateur:	compilateur.cpp tokeniser.o
	@echo "\e[32m--------| \e[1;32mCompilation of compilateur.cpp\e[0m\e[32m |--------\e[0m"
	g++ -ggdb -o compilateur compilateur.cpp tokeniser.o
	@echo "\e[32m\tDONE\e[0m\n"
test:		compilateur test.p
	@echo "\e[32m--------| \e[1mTest of compilateur\e[0m\e[32m |--------\e[0m"
	./compilateur <test.p >test.s
	gcc  -ggdb -no-pie -fno-pie test.s -o test
	@echo "\e[32m\tDONE\e[0m"
install:
	@echo "\e[32m--------| \e[1mInstallation of dep\e[0m\e[32m |--------\e[0m"
	sudo apt install flex flexc++ libfl-dev
	@echo "\e[32m\tDONE\e[0m"