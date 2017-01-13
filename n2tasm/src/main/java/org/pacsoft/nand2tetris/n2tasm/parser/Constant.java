package org.pacsoft.nand2tetris.n2tasm.parser;

public class Constant extends Symbol {
	private final static int CONST_MIN = 0;
	private final static int CONST_MAX = 32767;

	private final int value;

	public Constant(int value) throws IllegalValueException {
		if (value < CONST_MIN) {
			throw new IllegalValueException("Constant " + value + " is negative");
		}
		if (value > CONST_MAX) {
			throw new IllegalValueException("Constant " + value + " is too large (maximum is " + CONST_MAX +")");
		}
		this.value = value;
	}

	@Override
	public int getValue() {
		return value;
	}

}
