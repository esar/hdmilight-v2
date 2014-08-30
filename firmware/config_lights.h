/*

   Copyright (C) 2013 Stephen Robinson
  
   This file is part of HDMI-Light
  
   HDMI-Light is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.
  
   HDMI-Light is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this code (see the file names COPING).  
   If not, see <http://www.gnu.org/licenses/>.
  
*/

#define DEFINE_LIGHT(xmin, ymin, xmax, ymax, shift, output) \
		(((unsigned long)xmin   & 63) <<  0) | \
		(((unsigned long)ymin   & 63) <<  6) | \
		(((unsigned long)xmax   & 63) << 12) | \
		(((unsigned long)ymax   & 63) << 18) | \
		(((unsigned long)shift  & 15) << 24) | \
		(((unsigned long)output &  7) << 28)

#define LIGHT_XMIN(x)   (((x) >> 0)  & 63)
#define LIGHT_YMIN(x)   (((x) >> 6)  & 63)
#define LIGHT_XMAX(x)   (((x) >> 12) & 63)
#define LIGHT_YMAX(x)   (((x) >> 18) & 63)
#define LIGHT_SHIFT(x)  (((x) >> 24) & 15)
#define LIGHT_OUTPUT(x) (((x) >> 28) & 7)

static const unsigned long g_lightTable[] PROGMEM = 
{
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 0
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 1
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 2
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 3
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 4
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 5
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 6
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 7
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 8
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 9
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 10
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 11
	DEFINE_LIGHT(0, 0, 0, 0, 0, 7),    // 12

DEFINE_LIGHT( 59, 28,  59, 28, 0, 0),   // 13
DEFINE_LIGHT( 60, 28,  60, 28, 0, 0),   // 14
DEFINE_LIGHT( 61, 28,  61, 28, 0, 0),   // 15
DEFINE_LIGHT( 62, 28,  62, 28, 0, 0),   // 16
DEFINE_LIGHT( 63, 28,  63, 28, 0, 0),   // 17
	//DEFINE_LIGHT( 0, 32,  8, 32, 3, 0),   // 13
	//DEFINE_LIGHT( 0, 31,  8, 31, 3, 0),   // 14
	//DEFINE_LIGHT( 0, 30,  8, 30, 3, 0),   // 15
	//DEFINE_LIGHT( 0, 29,  8, 29, 3, 0),   // 16
	//DEFINE_LIGHT( 0, 28,  8, 28, 3, 0),   // 17
	DEFINE_LIGHT( 0, 27,  8, 27, 3, 0),   // 18
	DEFINE_LIGHT( 0, 26,  8, 26, 3, 0),   // 19
	DEFINE_LIGHT( 0, 25,  8, 25, 3, 0),   // 20
	DEFINE_LIGHT( 0, 24,  8, 24, 3, 0),   // 21
	DEFINE_LIGHT( 0, 23,  8, 23, 3, 0),   // 22
	DEFINE_LIGHT( 0, 22,  8, 22, 3, 0),   // 23
	DEFINE_LIGHT( 0, 21,  8, 21, 3, 0),   // 24
	DEFINE_LIGHT( 0, 20,  8, 20, 3, 0),   // 25
	DEFINE_LIGHT( 0, 19,  8, 19, 3, 0),   // 26
	DEFINE_LIGHT( 0, 18,  8, 18, 3, 0),   // 27
	DEFINE_LIGHT( 0, 17,  8, 17, 3, 0),   // 28
	DEFINE_LIGHT( 0, 16,  8, 16, 3, 0),   // 29
	DEFINE_LIGHT( 0, 15,  8, 15, 3, 0),   // 30
	DEFINE_LIGHT( 0, 14,  8, 14, 3, 0),   // 31
	DEFINE_LIGHT( 0, 13,  8, 13, 3, 0),   // 32
	DEFINE_LIGHT( 0, 12,  8, 12, 3, 0),   // 33
	DEFINE_LIGHT( 0, 11,  8, 11, 3, 0),   // 34
	DEFINE_LIGHT( 0, 10,  8, 10, 3, 0),   // 35
	DEFINE_LIGHT( 0,  9,  8,  9, 3, 0),   // 36
	DEFINE_LIGHT( 0,  8,  8,  8, 3, 0),   // 37
	DEFINE_LIGHT( 0,  7,  8,  7, 3, 0),   // 38
	DEFINE_LIGHT( 0,  6,  8,  6, 3, 0),   // 39
	DEFINE_LIGHT( 0,  5,  8,  5, 3, 0),   // 40
	DEFINE_LIGHT( 0,  4,  8,  4, 3, 0),   // 41
	DEFINE_LIGHT( 0,  3,  8,  3, 3, 0),   // 42
	DEFINE_LIGHT( 0,  2,  8,  2, 3, 0),   // 43
	DEFINE_LIGHT( 0,  1,  8,  1, 3, 0),   // 44

	DEFINE_LIGHT( 5,  0,  5,  8, 3, 0),   // 45
	DEFINE_LIGHT( 6,  0,  6,  8, 3, 0),   // 46
	DEFINE_LIGHT( 7,  0,  7,  8, 3, 0),   // 47
	DEFINE_LIGHT( 8,  0,  8,  8, 3, 0),   // 48
	DEFINE_LIGHT( 9,  0,  9,  8, 3, 0),   // 49
	DEFINE_LIGHT(10,  0, 10,  8, 3, 0),   // 50
	DEFINE_LIGHT(11,  0, 11,  8, 3, 0),   // 51
	DEFINE_LIGHT(12,  0, 12,  8, 3, 0),   // 52
	DEFINE_LIGHT(13,  0, 13,  8, 3, 0),   // 53
	DEFINE_LIGHT(14,  0, 14,  8, 3, 0),   // 54
	DEFINE_LIGHT(15,  0, 15,  8, 3, 0),   // 55
	DEFINE_LIGHT(16,  0, 16,  8, 3, 0),   // 56
	DEFINE_LIGHT(17,  0, 17,  8, 3, 0),   // 57
	DEFINE_LIGHT(18,  0, 18,  8, 3, 0),   // 58
	DEFINE_LIGHT(19,  0, 19,  8, 3, 0),   // 59
	DEFINE_LIGHT(20,  0, 20,  8, 3, 0),   // 60
	DEFINE_LIGHT(21,  0, 21,  8, 3, 0),   // 61
	DEFINE_LIGHT(22,  0, 22,  8, 3, 0),   // 62
	DEFINE_LIGHT(23,  0, 23,  8, 3, 0),   // 63
	DEFINE_LIGHT(24,  0, 24,  8, 3, 0),   // 64
	DEFINE_LIGHT(25,  0, 25,  8, 3, 0),   // 65
	DEFINE_LIGHT(26,  0, 26,  8, 3, 0),   // 66
	DEFINE_LIGHT(27,  0, 27,  8, 3, 0),   // 67
	DEFINE_LIGHT(28,  0, 28,  8, 3, 0),   // 68
	DEFINE_LIGHT(29,  0, 29,  8, 3, 0),   // 69
	DEFINE_LIGHT(30,  0, 30,  8, 3, 0),   // 70
	DEFINE_LIGHT(31,  0, 31,  8, 3, 0),   // 71
	DEFINE_LIGHT(32,  0, 32,  8, 3, 0),   // 72
	DEFINE_LIGHT(33,  0, 33,  8, 3, 0),   // 73
	DEFINE_LIGHT(34,  0, 34,  8, 3, 0),   // 74
	DEFINE_LIGHT(35,  0, 35,  8, 3, 0),   // 75
	DEFINE_LIGHT(36,  0, 36,  8, 3, 0),   // 76
	DEFINE_LIGHT(37,  0, 37,  8, 3, 0),   // 77
	DEFINE_LIGHT(38,  0, 38,  8, 3, 0),   // 78
	DEFINE_LIGHT(39,  0, 39,  8, 3, 0),   // 79
	DEFINE_LIGHT(40,  0, 40,  8, 3, 0),   // 80
	DEFINE_LIGHT(41,  0, 41,  8, 3, 0),   // 81
	DEFINE_LIGHT(42,  0, 42,  8, 3, 0),   // 82
	DEFINE_LIGHT(43,  0, 43,  8, 3, 0),   // 83
	DEFINE_LIGHT(44,  0, 44,  8, 3, 0),   // 84
	DEFINE_LIGHT(45,  0, 45,  8, 3, 0),   // 85
	DEFINE_LIGHT(46,  0, 46,  8, 3, 0),   // 86
	DEFINE_LIGHT(47,  0, 47,  8, 3, 0),   // 87
	DEFINE_LIGHT(48,  0, 48,  8, 3, 0),   // 88
	DEFINE_LIGHT(49,  0, 49,  8, 3, 0),   // 89
	DEFINE_LIGHT(50,  0, 50,  8, 3, 0),   // 90
	DEFINE_LIGHT(51,  0, 51,  8, 3, 0),   // 91
	DEFINE_LIGHT(52,  0, 52,  8, 3, 0),   // 92
	DEFINE_LIGHT(53,  0, 53,  8, 3, 0),   // 93
	DEFINE_LIGHT(54,  0, 54,  8, 3, 0),   // 94
	DEFINE_LIGHT(55,  0, 55,  8, 3, 0),   // 95
	DEFINE_LIGHT(56,  0, 56,  8, 3, 0),   // 96
	DEFINE_LIGHT(57,  0, 57,  8, 3, 0),   // 97
	DEFINE_LIGHT(58,  0, 58,  8, 3, 0),   // 98
	DEFINE_LIGHT(59,  0, 59,  8, 3, 0),   // 99
	DEFINE_LIGHT(60,  0, 60,  8, 3, 0),   // 100
	DEFINE_LIGHT(61,  0, 61,  8, 3, 0),   // 101

	DEFINE_LIGHT(53,  0, 60,  0, 3, 0),   // 102
	DEFINE_LIGHT(53,  1, 60,  1, 3, 0),   // 103
	DEFINE_LIGHT(53,  2, 60,  2, 3, 0),   // 104
	DEFINE_LIGHT(53,  3, 60,  3, 3, 0),   // 105
	DEFINE_LIGHT(53,  4, 60,  4, 3, 0),   // 106
	DEFINE_LIGHT(53,  5, 60,  5, 3, 0),   // 107
	DEFINE_LIGHT(53,  6, 60,  6, 3, 0),   // 108
	DEFINE_LIGHT(53,  7, 60,  7, 3, 0),   // 109
	DEFINE_LIGHT(53,  8, 60,  8, 3, 0),   // 110
	DEFINE_LIGHT(53,  9, 60,  9, 3, 0),   // 111
	DEFINE_LIGHT(53, 10, 60, 10, 3, 0),   // 112
	DEFINE_LIGHT(53, 11, 60, 11, 3, 0),   // 113
	DEFINE_LIGHT(53, 12, 60, 12, 3, 0),   // 114
	DEFINE_LIGHT(53, 13, 60, 13, 3, 0),   // 115
	DEFINE_LIGHT(53, 14, 60, 14, 3, 0),   // 116
	DEFINE_LIGHT(53, 15, 60, 15, 3, 0),   // 117
	DEFINE_LIGHT(53, 16, 60, 16, 3, 0),   // 118
	DEFINE_LIGHT(53, 17, 60, 17, 3, 0),   // 119
	DEFINE_LIGHT(53, 18, 60, 18, 3, 0),   // 120
	DEFINE_LIGHT(53, 19, 60, 19, 3, 0),   // 121
	DEFINE_LIGHT(53, 20, 60, 20, 3, 0),   // 122
	DEFINE_LIGHT(53, 21, 60, 21, 3, 0),   // 123
	DEFINE_LIGHT(53, 22, 60, 22, 3, 0),   // 124
	DEFINE_LIGHT(53, 23, 60, 23, 3, 0),   // 125
	DEFINE_LIGHT(53, 24, 60, 24, 3, 0),   // 126
	DEFINE_LIGHT(53, 25, 60, 25, 3, 0),   // 127
	DEFINE_LIGHT(53, 26, 60, 26, 3, 0),   // 128
	DEFINE_LIGHT(53, 27, 60, 27, 3, 0),   // 129
	DEFINE_LIGHT(53, 28, 60, 28, 3, 0),   // 130
	DEFINE_LIGHT(53, 29, 60, 29, 3, 0),   // 131
	DEFINE_LIGHT(53, 30, 60, 30, 3, 0),   // 132
	DEFINE_LIGHT(53, 31, 60, 31, 3, 0),   // 133


	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 134
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 135
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 136
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 137
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 138
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 139
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 140
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 141
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 142
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 143
	DEFINE_LIGHT( 0, 33,  8, 33, 3, 4),   // 144
	DEFINE_LIGHT( 0, 32,  8, 32, 3, 4),   // 145
	DEFINE_LIGHT( 0, 31,  8, 31, 3, 4),   // 146
	DEFINE_LIGHT( 0, 30,  8, 30, 3, 4),   // 147
	DEFINE_LIGHT( 0, 29,  8, 29, 3, 4),   // 148
	DEFINE_LIGHT( 0, 28,  8, 28, 3, 4),   // 149
	DEFINE_LIGHT( 0, 27,  8, 27, 3, 4),   // 150
	DEFINE_LIGHT( 0, 26,  8, 26, 3, 4),   // 151
	DEFINE_LIGHT( 0, 25,  8, 25, 3, 4),   // 152
	DEFINE_LIGHT( 0, 24,  8, 24, 3, 4),   // 153
	DEFINE_LIGHT( 0, 23,  8, 23, 3, 4),   // 154
	DEFINE_LIGHT( 0, 22,  8, 22, 3, 4),   // 155
	DEFINE_LIGHT( 0, 21,  8, 21, 3, 4),   // 156
	DEFINE_LIGHT( 0, 20,  8, 20, 3, 4),   // 157
	DEFINE_LIGHT( 0, 19,  8, 19, 3, 4),   // 158
	DEFINE_LIGHT( 0, 18,  8, 18, 3, 4),   // 159
	DEFINE_LIGHT( 0, 17,  8, 17, 3, 4),   // 160
	DEFINE_LIGHT( 0, 16,  8, 16, 3, 4),   // 161
	DEFINE_LIGHT( 0, 15,  8, 15, 3, 4),   // 162
	DEFINE_LIGHT( 0, 14,  8, 14, 3, 4),   // 163
	DEFINE_LIGHT( 0, 13,  8, 13, 3, 4),   // 164
	DEFINE_LIGHT( 0, 12,  8, 12, 3, 4),   // 165
	DEFINE_LIGHT( 0, 11,  8, 11, 3, 4),   // 166
	DEFINE_LIGHT( 0, 10,  8, 10, 3, 4),   // 167
	DEFINE_LIGHT( 0,  9,  8,  9, 3, 4),   // 168
	DEFINE_LIGHT( 0,  8,  8,  8, 3, 4),   // 169
	DEFINE_LIGHT( 0,  7,  8,  7, 3, 4),   // 170
	DEFINE_LIGHT( 0,  6,  8,  6, 3, 4),   // 171
	DEFINE_LIGHT( 0,  5,  8,  5, 3, 4),   // 172
	DEFINE_LIGHT( 0,  4,  8,  4, 3, 4),   // 173
	DEFINE_LIGHT( 0,  3,  8,  3, 3, 4),   // 174
	DEFINE_LIGHT( 0,  2,  8,  2, 3, 4),   // 175
	DEFINE_LIGHT( 0,  1,  8,  1, 3, 4),   // 176
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 220
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 221
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 222
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 223
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 224
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 225
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 226
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 227
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 228
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 229
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 230
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 231
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 232
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 233
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 234
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 235
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 236
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 4),   // 237

	DEFINE_LIGHT( 0,  0,  0,  0, 0, 2),   // 177
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 2),   // 178
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 2),   // 179
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 2),   // 180
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 2),   // 181
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 2),   // 182
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 2),   // 183
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 2),   // 184
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 2),   // 185
	DEFINE_LIGHT( 0,  0,  0,  0, 0, 2),   // 186
	DEFINE_LIGHT( 56, 33, 63, 33, 3, 2),   // 187
	DEFINE_LIGHT( 56, 32, 63, 32, 3, 2),   // 188
	DEFINE_LIGHT( 56, 31, 63, 31, 3, 2),   // 189
	DEFINE_LIGHT( 56, 30, 63, 30, 3, 2),   // 190
	DEFINE_LIGHT( 56, 29, 63, 29, 3, 2),   // 191
	DEFINE_LIGHT( 56, 28, 63, 28, 3, 2),   // 192
	DEFINE_LIGHT( 56, 27, 63, 27, 3, 2),   // 193
	DEFINE_LIGHT( 56, 26, 63, 26, 3, 2),   // 194
	DEFINE_LIGHT( 56, 25, 63, 25, 3, 2),   // 195
	DEFINE_LIGHT( 56, 24, 63, 24, 3, 2),   // 196
	DEFINE_LIGHT( 56, 23, 63, 23, 3, 2),   // 197
	DEFINE_LIGHT( 56, 22, 63, 22, 3, 2),   // 198
	DEFINE_LIGHT( 56, 21, 63, 21, 3, 2),   // 199
	DEFINE_LIGHT( 56, 20, 63, 20, 3, 2),   // 200
	DEFINE_LIGHT( 56, 19, 63, 19, 3, 2),   // 201
	DEFINE_LIGHT( 56, 18, 63, 18, 3, 2),   // 202
	DEFINE_LIGHT( 56, 17, 63, 17, 3, 2),   // 203
	DEFINE_LIGHT( 56, 16, 63, 16, 3, 2),   // 204
	DEFINE_LIGHT( 56, 15, 63, 15, 3, 2),   // 205
	DEFINE_LIGHT( 56, 14, 63, 14, 3, 2),   // 206
	DEFINE_LIGHT( 56, 13, 63, 13, 3, 2),   // 207
	DEFINE_LIGHT( 56, 12, 63, 12, 3, 2),   // 208
	DEFINE_LIGHT( 56, 11, 63, 11, 3, 2),   // 209
	DEFINE_LIGHT( 56, 10, 63, 10, 3, 2),   // 210
	DEFINE_LIGHT( 56,  9, 63,  9, 3, 2),   // 211
	DEFINE_LIGHT( 56,  8, 63,  8, 3, 2),   // 212
	DEFINE_LIGHT( 56,  7, 63,  7, 3, 2),   // 213
	DEFINE_LIGHT( 56,  6, 63,  6, 3, 2),   // 214
	DEFINE_LIGHT( 56,  5, 63,  5, 3, 2),   // 215
	DEFINE_LIGHT( 56,  4, 63,  4, 3, 2),   // 216
	DEFINE_LIGHT( 56,  3, 63,  3, 3, 2),   // 217
	DEFINE_LIGHT( 56,  2, 63,  2, 3, 2),   // 218
	DEFINE_LIGHT( 56,  1, 63,  1, 3, 2),   // 219

	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 238
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 239
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 240
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 241
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 242
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 243
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 244
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 245
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 246
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 247
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 248
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 249
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 250
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 251
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 252
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 253
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 254
	DEFINE_LIGHT(  0,  0,  0,  0, 0, 2),   // 255

	0xFFFFFFFF

/*
	{   0, 0, 0, 0, 0, 0 },
	{   1, 0, 0, 0, 0, 0 },
	{   2, 0, 0, 0, 0, 0 },
	{   3, 0, 0, 0, 0, 0 },
	{   4, 0, 0, 0, 0, 0 },
	{   5, 0, 0, 0, 0, 0 },
	{   6, 0, 0, 0, 0, 0 },
	{   7, 0, 0, 0, 0, 0 },
	{   8, 0, 0, 0, 0, 0 },
	{   9, 0, 0, 0, 0, 0 },
	{  10, 0, 0, 0, 0, 0 },
	{  11, 0, 0, 0, 0, 0 },
	{  12, 0, 0, 0, 0, 0 },

	{  13,  0, 32,  8, 32, 3 },
	{  14,  0, 31,  8, 31, 3 },
	{  15,  0, 30,  8, 30, 3 },
	{  16,  0, 29,  8, 29, 3 },
	{  17,  0, 28,  8, 28, 3 },
	{  18,  0, 27,  8, 27, 3 },
	{  19,  0, 26,  8, 26, 3 },
	{  20,  0, 25,  8, 25, 3 },
	{  21,  0, 24,  8, 24, 3 },
	{  22,  0, 23,  8, 23, 3 },
	{  23,  0, 22,  8, 22, 3 },
	{  24,  0, 21,  8, 21, 3 },
	{  25,  0, 20,  8, 20, 3 },
	{  26,  0, 19,  8, 19, 3 },
	{  27,  0, 18,  8, 18, 3 },
	{  28,  0, 17,  8, 17, 3 },
	{  29,  0, 16,  8, 16, 3 },
	{  30,  0, 15,  8, 15, 3 },
	{  31,  0, 14,  8, 14, 3 },
	{  32,  0, 13,  8, 13, 3 },
	{  33,  0, 12,  8, 12, 3 },
	{  34,  0, 11,  8, 11, 3 },
	{  35,  0, 10,  8, 10, 3 },
	{  36,  0,  9,  8,  9, 3 },
	{  37,  0,  8,  8,  8, 3 },
	{  38,  0,  7,  8,  7, 3 },
	{  39,  0,  6,  8,  6, 3 },
	{  40,  0,  5,  8,  5, 3 },
	{  41,  0,  4,  8,  4, 3 },
	{  42,  0,  3,  8,  3, 3 },
	{  43,  0,  2,  8,  2, 3 },
	{  44,  0,  1,  8,  1, 3 },

	{  45,  5,  0,  5,  8, 3 },
	{  46,  6,  0,  6,  8, 3 },
	{  47,  7,  0,  7,  8, 3 },
	{  48,  8,  0,  8,  8, 3 },
	{  49,  9,  0,  9,  8, 3 },
	{  50, 10,  0, 10,  8, 3 },
	{  51, 11,  0, 11,  8, 3 },
	{  52, 12,  0, 12,  8, 3 },
	{  53, 13,  0, 13,  8, 3 },
	{  54, 14,  0, 14,  8, 3 },
	{  55, 15,  0, 15,  8, 3 },
	{  56, 16,  0, 16,  8, 3 },
	{  57, 17,  0, 17,  8, 3 },
	{  58, 18,  0, 18,  8, 3 },
	{  59, 19,  0, 19,  8, 3 },
	{  60, 20,  0, 20,  8, 3 },
	{  61, 21,  0, 21,  8, 3 },
	{  62, 22,  0, 22,  8, 3 },
	{  63, 23,  0, 23,  8, 3 },
	{  64, 24,  0, 24,  8, 3 },
	{  65, 25,  0, 25,  8, 3 },
	{  66, 26,  0, 26,  8, 3 },
	{  67, 27,  0, 27,  8, 3 },
	{  68, 28,  0, 28,  8, 3 },
	{  69, 29,  0, 29,  8, 3 },
	{  70, 30,  0, 30,  8, 3 },
	{  71, 31,  0, 31,  8, 3 },
	{  72, 32,  0, 32,  8, 3 },
	{  73, 33,  0, 33,  8, 3 },
	{  74, 34,  0, 34,  8, 3 },
	{  75, 35,  0, 35,  8, 3 },
	{  76, 36,  0, 36,  8, 3 },
	{  77, 37,  0, 37,  8, 3 },
	{  78, 38,  0, 38,  8, 3 },
	{  79, 39,  0, 39,  8, 3 },
	{  80, 40,  0, 40,  8, 3 },
	{  81, 41,  0, 41,  8, 3 },
	{  82, 42,  0, 42,  8, 3 },
	{  83, 43,  0, 43,  8, 3 },
	{  84, 44,  0, 44,  8, 3 },
	{  85, 45,  0, 45,  8, 3 },
	{  86, 46,  0, 46,  8, 3 },
	{  87, 47,  0, 47,  8, 3 },
	{  88, 48,  0, 48,  8, 3 },
	{  89, 49,  0, 49,  8, 3 },
	{  90, 50,  0, 50,  8, 3 },
	{  91, 51,  0, 51,  8, 3 },
	{  92, 52,  0, 52,  8, 3 },
	{  93, 53,  0, 53,  8, 3 },
	{  94, 54,  0, 54,  8, 3 },
	{  95, 55,  0, 55,  8, 3 },
	{  96, 56,  0, 56,  8, 3 },
	{  97, 57,  0, 57,  8, 3 },
	{  98, 58,  0, 58,  8, 3 },
	{  99, 59,  0, 59,  8, 3 },
	{ 100, 60,  0, 60,  8, 3 },
	{ 101, 61,  0, 61,  8, 3 },

	{ 102, 53,  0, 60,  0, 3 },
	{ 103, 53,  1, 60,  1, 3 },
	{ 104, 53,  2, 60,  2, 3 },
	{ 105, 53,  3, 60,  3, 3 },
	{ 106, 53,  4, 60,  4, 3 },
	{ 107, 53,  5, 60,  5, 3 },
	{ 108, 53,  6, 60,  6, 3 },
	{ 109, 53,  7, 60,  7, 3 },
	{ 110, 53,  8, 60,  8, 3 },
	{ 111, 53,  9, 60,  9, 3 },
	{ 112, 53, 10, 60, 10, 3 },
	{ 113, 53, 11, 60, 11, 3 },
	{ 114, 53, 12, 60, 12, 3 },
	{ 115, 53, 13, 60, 13, 3 },
	{ 116, 53, 14, 60, 14, 3 },
	{ 117, 53, 15, 60, 15, 3 },
	{ 118, 53, 16, 60, 16, 3 },
	{ 119, 53, 17, 60, 17, 3 },
	{ 120, 53, 18, 60, 18, 3 },
	{ 121, 53, 19, 60, 19, 3 },
	{ 122, 53, 20, 60, 20, 3 },
	{ 123, 53, 21, 60, 21, 3 },
	{ 124, 53, 22, 60, 22, 3 },
	{ 125, 53, 23, 60, 23, 3 },
	{ 126, 53, 24, 60, 24, 3 },
	{ 127, 53, 25, 60, 25, 3 },
	{ 128, 53, 26, 60, 26, 3 },
	{ 129, 53, 27, 60, 27, 3 },
	{ 130, 53, 28, 60, 28, 3 },
	{ 131, 53, 29, 60, 29, 3 },
	{ 132, 53, 30, 60, 30, 3 },
	{ 133, 53, 31, 60, 31, 3 },

	{ 255, 0, 0, 0, 0, 0}
*/
};




