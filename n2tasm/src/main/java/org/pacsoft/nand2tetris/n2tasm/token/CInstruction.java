package org.pacsoft.nand2tetris.n2tasm.token;

public class CInstruction extends Instruction {
	private final String code;

	public CInstruction(String compCode, String destCode, String jumpCode) {
		code = "111" + compCode + destCode + jumpCode;
	}

	@Override
	public String getCode() {
		return code;
	}
}
