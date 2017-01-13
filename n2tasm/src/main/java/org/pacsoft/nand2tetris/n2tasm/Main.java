package org.pacsoft.nand2tetris.n2tasm;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;

import org.pacsoft.nand2tetris.n2tasm.instruction.Instruction;
import org.pacsoft.nand2tetris.n2tasm.parser.Parser;
import org.pacsoft.nand2tetris.n2tasm.parser.ParserException;

public class Main {

	private void go(InputStream is, PrintStream os, PrintStream err) throws AssemblerException, IOException {

		SymbolTable symbolTable = new SymbolTable();

		Parser parser = new Parser(symbolTable);

		BufferedReader br = new BufferedReader(new InputStreamReader(is));

		parser.parse(br);

		if (parser.isSuccesful()) {
			symbolTable.resolveAll();

			for(Instruction instr: parser.getInstructions()) {
				os.println(instr.getCode());
			}

			err.println("Assembly success");
		} else {
			err.println("Assembly failure");

			for(ParserException pe: parser.getErrors()) {
				err.println(pe.getLine() + ": " + pe.getMessage());
			};
		}
	}

	public static void main(String[] args) throws Exception {
		InputStream is = System.in;
		PrintStream os = System.out;
		PrintStream err = System.err;

		try {
			if (args.length > 0) {
				is = new FileInputStream(args[0]);
			}

			if (args.length > 1) {
				os = new PrintStream(new FileOutputStream(args[1]));
			}

			Main main = new Main();
			main.go(is, os, err);
		} catch (Exception ex) {
			ex.printStackTrace(err);
		} finally {
			if (os != System.out) {
				os.close();
			}
			if (is != System.in) {
				is.close();
			}
		}
	}
}
