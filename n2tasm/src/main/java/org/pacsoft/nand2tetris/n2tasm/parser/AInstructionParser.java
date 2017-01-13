package org.pacsoft.nand2tetris.n2tasm.parser;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.pacsoft.nand2tetris.n2tasm.SymbolTable;
import org.pacsoft.nand2tetris.n2tasm.instruction.AInstruction;

public class AInstructionParser {
	private final static Pattern INTEGER_PATTERN = Pattern.compile("^[0-9]+$");
	private final static Pattern LITERAL_PATTERN = Pattern.compile("^[a-zA-Z_.$:][a-zA-Z_.$:0-9]+$");

	public static AInstruction parse(int lineNo, String value, SymbolTable symbolTable) throws ParserException {
		Matcher mm = INTEGER_PATTERN.matcher(value);
		if (mm.matches()) {
			int intValue = Integer.valueOf(value);
			Constant constValue = new Constant(intValue);

			AInstruction instr = new AInstruction(constValue);

			return instr;
		}

		mm = LITERAL_PATTERN.matcher(value);
		if (mm.matches()) {
			String name = value;
			Symbol symbol = symbolTable.referenceSymbol(lineNo, name);

			AInstruction instr = new AInstruction(symbol);

			return instr;
		}

		throw new SyntaxErrorException("Bad A instruction argument \"" + value + "\"");
	}
}
