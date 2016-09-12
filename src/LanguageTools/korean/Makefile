#  Korean Morphological Analyze   -c -g -Ddebug
#	kma  input_text output_text

kma: Main.cpp Source/KMAnalyzer.a
	g++ -o  kma Main.cpp Source/KMAnalyzer.a

Source/KMAnalyzer.a: Source/*.cpp
	cd Source; g++ -c  -D_USRDLL *.cpp
	cd Source; ar -r KMAnalyzer.a *.o

    		
