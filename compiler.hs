import System.Directory
import System.Environment
import System.Exit
import System.IO
import System.Cmd

symbols = ['<', '>', ',', '.', '+', '-', '[', ']']

main = do args <- getArgs;
       	  case args of
	       	    --[infile] -> do
	       	    [outfile, infile] -> do text <- get infile
		    	      	      	    let outtext = compile $ filter (\x -> x `elem` symbols) text
					    --putStrLn outtext
					    --hFlush stdout
					    (path, handle) <- openTempFile "." "tmp.c"
					    hClose handle
					    writeFile path outtext
					    system $ "gcc -o " ++ outfile ++ " " ++ path
					    removeFile path
					    exitWith ExitSuccess
		    _ -> do error "Two arguments expected: outfile and infile."
		      	    exitWith $ ExitFailure (-1)

replace '+' = "  memory[memptr]++;\n"
replace '-' = "  memory[memptr]--;\n"
replace '>' = "  memptr = (memptr + 1) % SIZE;\n"
replace '<' = "  memptr = (memptr + SIZE - 1) % SIZE;\n"
replace '.' = "  putchar(memory[memptr]);\n"
replace ',' = "  memory[memptr] = getchar();\n"
replace '[' = "  while(memory[memptr]) {\n"
replace ']' = "  }\n"

stdtext = "#include <stdio.h>\n#include <stdlib.h>\n\n#define SIZE 65536\n\nchar memory[SIZE];\nint memptr = 0;\n\nint main() {\n\n"

compile :: String -> String
compile intext = stdtext ++ (foldr (++) "" $ map replace intext) ++ "\n}"
--let compiled = unwords $ map replace intext
--	       	 stdtext ++ compiled ++ "}"

get :: String -> IO String
get infile = do input <- openFile infile ReadMode;
    	     	hGetContents input

