package org.pacsoft.nand2tetris.n2tasm.parser;

public class Location extends Symbol {
	public enum Type {CODE, DATA, BUILT_IN, UNKNOWN};

	private final int declarationLine;
	private final String name;
	private Type type;
	private int value;

	public Location(int declarationLine, String name) {
		this.declarationLine = declarationLine;
		this.name = name;
		type = Type.UNKNOWN;
	}

	public String getName() {
		return name;
	}

	public int getDeclarationLine() {
		return declarationLine;
	}

	public Type getType() {
		return type;
	}

	public void setType(Type type) {
		this.type = type;
	}

	@Override
	public int getValue() {
		return value;
	}

	public void setValue(int value) {
		this.value = value;
	}
}
