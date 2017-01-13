package org.pacsoft.nand2tetris.n2tasm.parser;

import org.pacsoft.nand2tetris.n2tasm.AssemblerException;

public class ParserException extends AssemblerException {
	private static final long serialVersionUID = -952012404684142624L;

	private int line;

	public ParserException() {
		super();
		// TODO Auto-generated constructor stub
	}

	public ParserException(String message, Throwable cause) {
		super(message, cause);
		// TODO Auto-generated constructor stub
	}

	public ParserException(String message) {
		super(message);
		// TODO Auto-generated constructor stub
	}

	public ParserException(Throwable cause) {
		super(cause);
		// TODO Auto-generated constructor stub
	}

	public void setLine(int line) {
		this.line = line;
	}

	public int getLine() {
		return line;
	}
}
