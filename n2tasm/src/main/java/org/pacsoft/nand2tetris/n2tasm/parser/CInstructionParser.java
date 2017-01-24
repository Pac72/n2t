package org.pacsoft.nand2tetris.n2tasm.parser;

import java.util.HashMap;
import java.util.Map;

import org.pacsoft.nand2tetris.n2tasm.token.CInstruction;

public class CInstructionParser {
	private static class LazyHolder {
		private static final CInstructionParser INSTANCE = new CInstructionParser();
	}

	private final Map<String, String> compCodeMap = new HashMap<String, String>();
	private final Map<String, String> destCodeMap = new HashMap<String, String>();
	private final Map<String, String> jumpCodeMap = new HashMap<String, String>();

	private CInstructionParser() {
		initializeCompCodeMap();
		initializeDestCodeMap();
		initializeJumpCodeMap();
	}

	private void initializeCompCodeMap() {
		compCodeMap.put("0", "0101010");
		compCodeMap.put("1", "0111111");
		compCodeMap.put("-1", "0111010");
		compCodeMap.put("D", "0001100");
		compCodeMap.put("A", "0110000");
		compCodeMap.put("!D", "0001101");
		compCodeMap.put("!A", "0110001");
		compCodeMap.put("-D", "0001111");
		compCodeMap.put("-A", "0110011");
		compCodeMap.put("D+1", "0011111");
		compCodeMap.put("A+1", "0110111");
		compCodeMap.put("D-1", "0001110");
		compCodeMap.put("A-1", "0110010");
		compCodeMap.put("D+A", "0000010");
		compCodeMap.put("A+D", "0000010");
		compCodeMap.put("D-A", "0010011");
		compCodeMap.put("A-D", "0000111");
		compCodeMap.put("D&A", "0000000");
		compCodeMap.put("A&D", "0000000");
		compCodeMap.put("D|A", "0010101");
		compCodeMap.put("A|D", "0010101");
		compCodeMap.put("M", "1110000");
		compCodeMap.put("!M", "1110001");
		compCodeMap.put("-M", "1110011");
		compCodeMap.put("M+1", "1110111");
		compCodeMap.put("M-1", "1110010");
		compCodeMap.put("D+M", "1000010");
		compCodeMap.put("M+D", "1000010");
		compCodeMap.put("D-M", "1010011");
		compCodeMap.put("M-D", "1000111");
		compCodeMap.put("D&M", "1000000");
		compCodeMap.put("M&D", "1000000");
		compCodeMap.put("D|M", "1010101");
		compCodeMap.put("M|D", "1010101");
	}

	private void initializeDestCodeMap() {
		destCodeMap.put(null, "000");
		destCodeMap.put("M", "001");
		destCodeMap.put("D", "010");
		destCodeMap.put("MD", "011");
		destCodeMap.put("DM", "011");
		destCodeMap.put("A", "100");
		destCodeMap.put("AM", "101");
		destCodeMap.put("MA", "101");
		destCodeMap.put("AD", "110");
		destCodeMap.put("DA", "110");
		destCodeMap.put("ADM", "111");
		destCodeMap.put("AMD", "111");
		destCodeMap.put("DAM", "111");
		destCodeMap.put("DMA", "111");
		destCodeMap.put("MAD", "111");
		destCodeMap.put("MDA", "111");
	}

	private void initializeJumpCodeMap() {
		jumpCodeMap.put(null, "000");
		jumpCodeMap.put("JGT", "001");
		jumpCodeMap.put("JEQ", "010");
		jumpCodeMap.put("JGE", "011");
		jumpCodeMap.put("JLT", "100");
		jumpCodeMap.put("JNE", "101");
		jumpCodeMap.put("JLE", "110");
		jumpCodeMap.put("JMP", "111");
	}

	public static CInstruction parse(String dest, String comp, String jump) throws SyntaxErrorException {
		String compCode = LazyHolder.INSTANCE.compCodeMap.get(comp);
		if (null == compCode) {
			throw new SyntaxErrorException("Wrong comp field \"" + comp + "\"");
		}
		String destCode = LazyHolder.INSTANCE.destCodeMap.get(dest);
		if (null == destCode) {
			throw new SyntaxErrorException("Wrong dest field \"" + dest + "\"");
		}
		String jumpCode = LazyHolder.INSTANCE.jumpCodeMap.get(jump);
		if (null == jumpCode) {
			throw new SyntaxErrorException("Wrong jump field \"" + jump + "\"");
		}

		CInstruction instr = new CInstruction(compCode, destCode, jumpCode);

		return instr;
	}
}
