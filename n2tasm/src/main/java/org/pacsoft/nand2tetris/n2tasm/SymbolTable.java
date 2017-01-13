package org.pacsoft.nand2tetris.n2tasm;

import java.util.HashMap;
import java.util.Map;

import org.pacsoft.nand2tetris.n2tasm.parser.DuplicateLabelException;
import org.pacsoft.nand2tetris.n2tasm.parser.IllegalNameException;
import org.pacsoft.nand2tetris.n2tasm.parser.Location;
import org.pacsoft.nand2tetris.n2tasm.parser.ParserException;

public class SymbolTable {
	private static final int FIRST_DATA_ADDRESS = 16;

	private final Map<String, Location> locations;

	public SymbolTable() {
		locations = new HashMap<String, Location>();
		populateBuiltIns();
	}

	private void createBuiltIn(String name, int value) {
		Location location = new Location(0, name);
		location.setType(Location.Type.BUILT_IN);
		location.setValue(value);
		locations.put(name, location);
	}

	private void populateBuiltIns() {
		createBuiltIn("SP", 0);
		createBuiltIn("LCL", 1);
		createBuiltIn("ARG", 2);
		createBuiltIn("THIS", 3);
		createBuiltIn("THAT", 4);
		createBuiltIn("SCREEN", 16384);
		createBuiltIn("KBD", 24576);

		for(int ii = 0; ii < 16; ii++) {
			createBuiltIn("R" + ii, ii);
		}
	}

//	public Location referenceVariable(int lineNumber, String name) throws ParserException {
//		Location location = locations.get(name);
//
//		if (location != null) {
//			if (location.getType() == Location.Type.CODE) {
//				// this is an already defined label
//				throw new IllegalNameException("Symbol " + name + " already defined as code location at line " + location.getDeclarationLine());
//			} else if (location.getType() == Location.Type.UNKNOWN) {
//				location.setType(Location.Type.DATA);
//			}
//		} else {
//			location = new Location(lineNumber, name);
//			locations.put(name, location);
//			location.setType(Location.Type.DATA);
//		}
//
//		return location;
//	}

	public Location defineLabel(int lineNumber, String name, int address) throws ParserException {
		Location location = locations.get(name);

		if (location != null) {
			if (location.getType() == Location.Type.CODE) {
				throw new DuplicateLabelException("Label " + name + " already defined at line " + location.getDeclarationLine());
			} else if (location.getType() == Location.Type.BUILT_IN) {
				throw new IllegalNameException("Symbol " + name + " is reserved");
			} else if (location.getType() == Location.Type.DATA) {
				throw new IllegalNameException("Symbol " + name + " already defined as data location at line " + location.getDeclarationLine());
			}
		} else {
			location = new Location(lineNumber, name);
			locations.put(name, location);
		}

		location.setType(Location.Type.CODE);
		location.setValue(address);

		return location;
	}

	public Location referenceSymbol(int lineNumber, String name) throws ParserException {
		Location location = locations.get(name);

		if (null == location) {
			location = new Location(lineNumber, name);
			locations.put(name, location);
		}

		return location;
	}

	public int resolveAll() {
		int currentDataAddress = FIRST_DATA_ADDRESS;
		for (Location location: locations.values()) {
			if (location.getType() == Location.Type.UNKNOWN || location.getType() == Location.Type.DATA) {
				location.setType(Location.Type.DATA);
				location.setValue(currentDataAddress++);
			}
		}

		return currentDataAddress;
	}
}
