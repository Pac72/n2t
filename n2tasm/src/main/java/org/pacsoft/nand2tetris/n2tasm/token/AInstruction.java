package org.pacsoft.nand2tetris.n2tasm.token;

import org.pacsoft.nand2tetris.n2tasm.parser.Symbol;

public class AInstruction extends Instruction {
	private final Symbol symbol;

	public AInstruction(Symbol symbol) {
		this.symbol = symbol;
	}

	@Override
	public String getCode() {
		int value = symbol.getValue();
		String unpaddedValueString = Integer.toBinaryString(value);
		String valueString =
				"000000000000000".substring(unpaddedValueString.length()) +
				unpaddedValueString;
		String code = "0" + valueString;
		return code;
	}

}
