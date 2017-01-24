package org.pacsoft.nand2tetris.n2tasm.token;

public class Label extends Token {
	private final String name;
	
	public Label(String name) {
		this.name = name;
	}

	public String getName() {
		return name;
	}
}
