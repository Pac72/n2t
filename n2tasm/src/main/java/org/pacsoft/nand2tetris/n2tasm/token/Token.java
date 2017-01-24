package org.pacsoft.nand2tetris.n2tasm.token;

public abstract class Token {
	private int lineNo;
	private String originalAssembler;

	public void setLine(int lineNo) {
		this.lineNo = lineNo;
	}

	public int getLine() {
		return lineNo;
	}

	public void setOriginalAssembler(String originalAssembler) {
		this.originalAssembler = originalAssembler;
	}

	public String getOriginalAssembler() {
		return originalAssembler;
	}
}
