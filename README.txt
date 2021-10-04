README

Included in the zip are the following inputs:
String	 		Strain name 			Input sequence file 	Sequence length (in bp)
input1 			COVID-19 China 			FASTA 					29,903
input2 			COVID-19 USA 			FASTA 					29,882
input3 			COVID-19 Australia 		FASTA 					29,893
input4 			COVID-19 India 			FASTA 					29,854
input5 			COVID-19 Brazil 		FASTA 					29,876
input6 			SARS_2013 				FASTA 					29,644
input7 			SARS_2017 				FASTA 					29,727
input8 			MERS_2012_Saudi 		FASTA 					30,056
input9 			MERS_2014_Saudi 		FASTA 					30,123
input10 		MERS_2014_USA 			FASTA 					30,123

The zip has a file "fingerprints.txt" for the unique fingerprints for each of the
10 input FASTA files

Two Alphabet files "EnglishAlphabet.txt" and "DNAAlphabet.txt" are provided

To Compile:
	gcc PA2-CMake.c PA2-CMake.h
To Run:
	Windows:
		a.exe <Alphabet.txt> <fasta1.txt> <fasta2.txt> ... <fastan.txt> NULL
	Linux:
		./a.out <Alphabet.txt> <fasta1.txt> <fasta2.txt> ... <inputn.txt> NULL