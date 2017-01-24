package org.pacsoft.nand2tetris.n2tasm;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;

import org.pacsoft.nand2tetris.n2tasm.parser.Location;
import org.pacsoft.nand2tetris.n2tasm.parser.Parser;
import org.pacsoft.nand2tetris.n2tasm.parser.ParserException;
import org.pacsoft.nand2tetris.n2tasm.token.Instruction;
import org.pacsoft.nand2tetris.n2tasm.token.Label;
import org.pacsoft.nand2tetris.n2tasm.token.Token;

public class Main {

	private void go(String asmFilename, String hackFilename, String hackdbgFilename) throws AssemblerException, IOException {
		try (InputStream asmInputStream = new FileInputStream(asmFilename)) {

			SymbolTable symbolTable = new SymbolTable();

			Parser parser = new Parser(symbolTable);

			BufferedReader br = new BufferedReader(new InputStreamReader(asmInputStream));

			parser.parse(br);

			if (parser.isSuccesful()) {
				symbolTable.resolveAll();

				try (PrintStream hackPrintStream = new PrintStream(new FileOutputStream(hackFilename))) {
					for(Token token: parser.getTokens()) {
						if (token instanceof Instruction) {
							Instruction instr = (Instruction)token;
							hackPrintStream.println(instr.getCode());
						}
					}
				}

				try (PrintStream hackdbgPrintStream = new PrintStream(new FileOutputStream(hackdbgFilename))) {
					int currLocation = 0;
					for(Location loc: symbolTable.getDataLocations()) {
						hackdbgPrintStream.println("D\t" + loc.getName() + "\t" + loc.getValue());
					}
					for(Token token: parser.getTokens()) {
						if (token instanceof Instruction) {
							Instruction instr = (Instruction)token;
							hackdbgPrintStream.println("C\t\t" + currLocation + "\t" + instr.getOriginalAssembler());
							currLocation++;
						} else {
							Label lbl = (Label) token;
							hackdbgPrintStream.println("L\t" + lbl.getName() + "\t" + currLocation + "\t" + lbl.getOriginalAssembler());
						}
					}
				}

				System.err.println("Assembly success");
			} else {
				System.err.println("Assembly failure");

				for(ParserException pe: parser.getErrors()) {
					System.err.println("line " + pe.getLine() + ": " + pe.getMessage());
				};
			}
		}
	}

	public static void main(String[] args) throws Exception {
		String asmFilename;
		String hackFilename;
		String hackdbgFilename;

		try {
			if (args.length < 1) {
				System.err.println("Usage: " + Main.class.getCanonicalName() + "<in_filename.asm> [<out_filename.hack> [<out_filename.hackdbg>]]");
				return;
			}
			asmFilename = args[0];

			if (args.length > 1) {
				hackFilename = args[1];
			} else {
				int ii = asmFilename.lastIndexOf('.');
				if (ii < 0) {
					ii = asmFilename.length();
				}
				hackFilename = asmFilename.substring(0, ii) + ".hack";
			}

			if (args.length > 2) {
				hackdbgFilename = args[2];
			} else {
				int ii = hackFilename.lastIndexOf('.');
				if (ii < 0) {
					ii = hackFilename.length();
				}
				hackdbgFilename = hackFilename.substring(0, ii) + ".hackdbg";
			}

			Main main = new Main();
			main.go(asmFilename, hackFilename, hackdbgFilename);
		} catch (Exception ex) {
			ex.printStackTrace(System.err);
		}
	}
}
