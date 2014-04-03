package com.mingspy.utils.dat.alph;

/**
 * A rough alphabet, use Unicode coding rules directly.<br>
 * 
 * In Unicode<br>
 * 0~256 are English coding area <br>
 * 0x4e00 ~ 0x9fbf are Chinese <br>
 * ff00 ~ ff5f are full-width number and chars<br>
 * 
 * @author xiuleili
 * 
 */
public class RoughAlphabet extends Alphabet {

	private static final long serialVersionUID = -5639241014698992119L;
	private final static int ChinesBase = 256;
	private final static int FullWidthNumBase = 0x9fbf - 0x4e00 + 256;
	private final static int WellSupportedLength = 0x9fbf - 0x4e00 + 256 + 0xff5f - 0xff00;
	private final static int LatinBase = 0x9fbf - 0x4e00 + 256 + 0xffef - 0xff00;

	@Override
	public int childSize(int ch) {
		return 65535;
	}

	@Override
	public int getInnerCode(int ch) {
		if (ch >= 0 && ch < 256) {
			return ch;
		} else if (ch >= 0x4e00 && ch <= 0x9fbf) {
			return ChinesBase + ch - 0x4e00;
		} else if (ch >= 0xff00 && ch <= 0xff5f) {
			return FullWidthNumBase + ch - 0xff00;
		} else if (ch > 0x9fbf) {
			return WellSupportedLength + ch - 0x9fbf;
		} else if (ch < 0x4e00) {
			return LatinBase + ch - 256;
		}

		throw new RuntimeException("Not supported:" + ch);

	}
}
