package org.pacsoft.nand2tetris.n2tasm.instruction;

public abstract class Instruction {
	private int lineNo;

	public void setLine(int lineNo) {
		this.lineNo = lineNo;
	}

	public int getLine() {
		return lineNo;
	}

	public abstract String getCode();
}
