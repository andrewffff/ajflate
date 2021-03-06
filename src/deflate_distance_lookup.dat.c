
static const unsigned char deflate_distance_lookup[] = {
 192,
 63,
 192,
 191,
 192,
 127,
 192,
 255,
 192,
 31,
 192,
 159,
 192,
 95,
 192,
 223,
 192,
 15,
 192,
 79,
 192,
 143,
 192,
 207,
 192,
 47,
 192,
 111,
 192,
 175,
 192,
 239,
 192,
 7,
 192,
 39,
 192,
 71,
 192,
 103,
 192,
 135,
 192,
 167,
 192,
 199,
 192,
 231,
 192,
 23,
 192,
 55,
 192,
 87,
 192,
 119,
 192,
 151,
 192,
 183,
 192,
 215,
 192,
 247,
 192,
 3,
 192,
 19,
 192,
 35,
 192,
 51,
 192,
 67,
 192,
 83,
 192,
 99,
 192,
 115,
 192,
 131,
 192,
 147,
 192,
 163,
 192,
 179,
 192,
 195,
 192,
 211,
 192,
 227,
 192,
 243,
 192,
 11,
 192,
 27,
 192,
 43,
 192,
 59,
 192,
 75,
 192,
 91,
 192,
 107,
 192,
 123,
 192,
 139,
 192,
 155,
 192,
 171,
 192,
 187,
 192,
 203,
 192,
 219,
 192,
 235,
 192,
 251,
 192,
 1,
 192,
 9,
 192,
 17,
 192,
 25,
 192,
 33,
 192,
 41,
 192,
 49,
 192,
 57,
 192,
 65,
 192,
 73,
 192,
 81,
 192,
 89,
 192,
 97,
 192,
 105,
 192,
 113,
 192,
 121,
 192,
 129,
 192,
 137,
 192,
 145,
 192,
 153,
 192,
 161,
 192,
 169,
 192,
 177,
 192,
 185,
 192,
 193,
 192,
 201,
 192,
 209,
 192,
 217,
 192,
 225,
 192,
 233,
 192,
 241,
 192,
 249,
 192,
 5,
 192,
 13,
 192,
 21,
 192,
 29,
 192,
 37,
 192,
 45,
 192,
 53,
 192,
 61,
 192,
 69,
 192,
 77,
 192,
 85,
 192,
 93,
 192,
 101,
 192,
 109,
 192,
 117,
 192,
 125,
 192,
 133,
 192,
 141,
 192,
 149,
 192,
 157,
 192,
 165,
 192,
 173,
 192,
 181,
 192,
 189,
 192,
 197,
 192,
 205,
 192,
 213,
 192,
 221,
 192,
 229,
 192,
 237,
 192,
 245,
 192,
 253,
 192,
 0,
 192,
 4,
 192,
 8,
 192,
 12,
 192,
 16,
 192,
 20,
 192,
 24,
 192,
 28,
 192,
 32,
 192,
 36,
 192,
 40,
 192,
 44,
 192,
 48,
 192,
 52,
 192,
 56,
 192,
 60,
 192,
 64,
 192,
 68,
 192,
 72,
 192,
 76,
 192,
 80,
 192,
 84,
 192,
 88,
 192,
 92,
 192,
 96,
 192,
 100,
 192,
 104,
 192,
 108,
 192,
 112,
 192,
 116,
 192,
 120,
 192,
 124,
 192,
 128,
 192,
 132,
 192,
 136,
 192,
 140,
 192,
 144,
 192,
 148,
 192,
 152,
 192,
 156,
 192,
 160,
 192,
 164,
 192,
 168,
 192,
 172,
 192,
 176,
 192,
 180,
 192,
 184,
 192,
 188,
 192,
 192,
 192,
 196,
 192,
 200,
 192,
 204,
 192,
 208,
 192,
 212,
 192,
 216,
 192,
 220,
 192,
 224,
 192,
 228,
 192,
 232,
 192,
 236,
 192,
 240,
 192,
 244,
 192,
 248,
 192,
 252,
 192,
 2,
 192,
 6,
 192,
 10,
 192,
 14,
 192,
 18,
 192,
 22,
 192,
 26,
 192,
 30,
 192,
 34,
 192,
 38,
 192,
 42,
 192,
 46,
 192,
 50,
 192,
 54,
 192,
 58,
 192,
 62,
 192,
 66,
 192,
 70,
 192,
 74,
 192,
 78,
 192,
 82,
 192,
 86,
 192,
 90,
 192,
 94,
 192,
 98,
 192,
 102,
 192,
 106,
 192,
 110,
 192,
 114,
 192,
 118,
 192,
 122,
 192,
 126,
 192,
 130,
 192,
 134,
 192,
 138,
 192,
 142,
 192,
 146,
 192,
 150,
 192,
 154,
 192,
 158,
 192,
 162,
 192,
 166,
 192,
 170,
 192,
 174,
 192,
 178,
 192,
 182,
 192,
 186,
 192,
 190,
 192,
 194,
 192,
 198,
 192,
 202,
 192,
 206,
 192,
 210,
 192,
 214,
 192,
 218,
 192,
 222,
 192,
 226,
 192,
 230,
 192,
 234,
 192,
 238,
 192,
 242,
 192,
 246,
 192,
 250,
 192,
 254,
 64,
 0,
 64,
 2,
 64,
 4,
 64,
 6,
 64,
 8,
 64,
 10,
 64,
 12,
 64,
 14,
 64,
 16,
 64,
 18,
 64,
 20,
 64,
 22,
 64,
 24,
 64,
 26,
 64,
 28,
 64,
 30,
 64,
 32,
 64,
 34,
 64,
 36,
 64,
 38,
 64,
 40,
 64,
 42,
 64,
 44,
 64,
 46,
 64,
 48,
 64,
 50,
 64,
 52,
 64,
 54,
 64,
 56,
 64,
 58,
 64,
 60,
 64,
 62,
 64,
 64,
 64,
 66,
 64,
 68,
 64,
 70,
 64,
 72,
 64,
 74,
 64,
 76,
 64,
 78,
 64,
 80,
 64,
 82,
 64,
 84,
 64,
 86,
 64,
 88,
 64,
 90,
 64,
 92,
 64,
 94,
 64,
 96,
 64,
 98,
 64,
 100,
 64,
 102,
 64,
 104,
 64,
 106,
 64,
 108,
 64,
 110,
 64,
 112,
 64,
 114,
 64,
 116,
 64,
 118,
 64,
 120,
 64,
 122,
 64,
 124,
 64,
 126,
 64,
 128,
 64,
 130,
 64,
 132,
 64,
 134,
 64,
 136,
 64,
 138,
 64,
 140,
 64,
 142,
 64,
 144,
 64,
 146,
 64,
 148,
 64,
 150,
 64,
 152,
 64,
 154,
 64,
 156,
 64,
 158,
 64,
 160,
 64,
 162,
 64,
 164,
 64,
 166,
 64,
 168,
 64,
 170,
 64,
 172,
 64,
 174,
 64,
 176,
 64,
 178,
 64,
 180,
 64,
 182,
 64,
 184,
 64,
 186,
 64,
 188,
 64,
 190,
 64,
 192,
 64,
 194,
 64,
 196,
 64,
 198,
 64,
 200,
 64,
 202,
 64,
 204,
 64,
 206,
 64,
 208,
 64,
 210,
 64,
 212,
 64,
 214,
 64,
 216,
 64,
 218,
 64,
 220,
 64,
 222,
 64,
 224,
 64,
 226,
 64,
 228,
 64,
 230,
 64,
 232,
 64,
 234,
 64,
 236,
 64,
 238,
 64,
 240,
 64,
 242,
 64,
 244,
 64,
 246,
 64,
 248,
 64,
 250,
 64,
 252,
 64,
 254,
 64,
 1,
 64,
 3,
 64,
 5,
 64,
 7,
 64,
 9,
 64,
 11,
 64,
 13,
 64,
 15,
 64,
 17,
 64,
 19,
 64,
 21,
 64,
 23,
 64,
 25,
 64,
 27,
 64,
 29,
 64,
 31,
 64,
 33,
 64,
 35,
 64,
 37,
 64,
 39,
 64,
 41,
 64,
 43,
 64,
 45,
 64,
 47,
 64,
 49,
 64,
 51,
 64,
 53,
 64,
 55,
 64,
 57,
 64,
 59,
 64,
 61,
 64,
 63,
 64,
 65,
 64,
 67,
 64,
 69,
 64,
 71,
 64,
 73,
 64,
 75,
 64,
 77,
 64,
 79,
 64,
 81,
 64,
 83,
 64,
 85,
 64,
 87,
 64,
 89,
 64,
 91,
 64,
 93,
 64,
 95,
 64,
 97,
 64,
 99,
 64,
 101,
 64,
 103,
 64,
 105,
 64,
 107,
 64,
 109,
 64,
 111,
 64,
 113,
 64,
 115,
 64,
 117,
 64,
 119,
 64,
 121,
 64,
 123,
 64,
 125,
 64,
 127,
 64,
 129,
 64,
 131,
 64,
 133,
 64,
 135,
 64,
 137,
 64,
 139,
 64,
 141,
 64,
 143,
 64,
 145,
 64,
 147,
 64,
 149,
 64,
 151,
 64,
 153,
 64,
 155,
 64,
 157,
 64,
 159,
 64,
 161,
 64,
 163,
 64,
 165,
 64,
 167,
 64,
 169,
 64,
 171,
 64,
 173,
 64,
 175,
 64,
 177,
 64,
 179,
 64,
 181,
 64,
 183,
 64,
 185,
 64,
 187,
 64,
 189,
 64,
 191,
 64,
 193,
 64,
 195,
 64,
 197,
 64,
 199,
 64,
 201,
 64,
 203,
 64,
 205,
 64,
 207,
 64,
 209,
 64,
 211,
 64,
 213,
 64,
 215,
 64,
 217,
 64,
 219,
 64,
 221,
 64,
 223,
 64,
 225,
 64,
 227,
 64,
 229,
 64,
 231,
 64,
 233,
 64,
 235,
 64,
 237,
 64,
 239,
 64,
 241,
 64,
 243,
 64,
 245,
 64,
 247,
 64,
 249,
 64,
 251,
 64,
 253,
 64,
 255,
 0,
 0,
 0,
 1,
 0,
 2,
 0,
 3,
 0,
 4,
 0,
 5,
 0,
 6,
 0,
 7,
 0,
 8,
 0,
 9,
 0,
 10,
 0,
 11,
 0,
 12,
 0,
 13,
 0,
 14,
 0,
 15,
 0,
 16,
 0,
 17,
 0,
 18,
 0,
 19,
 0,
 20,
 0,
 21,
 0,
 22,
 0,
 23,
 0,
 24,
 0,
 25,
 0,
 26,
 0,
 27,
 0,
 28,
 0,
 29,
 0,
 30,
 0,
 31,
 0,
 32,
 0,
 33,
 0,
 34,
 0,
 35,
 0,
 36,
 0,
 37,
 0,
 38,
 0,
 39,
 0,
 40,
 0,
 41,
 0,
 42,
 0,
 43,
 0,
 44,
 0,
 45,
 0,
 46,
 0,
 47,
 0,
 48,
 0,
 49,
 0,
 50,
 0,
 51,
 0,
 52,
 0,
 53,
 0,
 54,
 0,
 55,
 0,
 56,
 0,
 57,
 0,
 58,
 0,
 59,
 0,
 60,
 0,
 61,
 0,
 62,
 0,
 63,
 0,
 64,
 0,
 65,
 0,
 66,
 0,
 67,
 0,
 68,
 0,
 69,
 0,
 70,
 0,
 71,
 0,
 72,
 0,
 73,
 0,
 74,
 0,
 75,
 0,
 76,
 0,
 77,
 0,
 78,
 0,
 79,
 0,
 80,
 0,
 81,
 0,
 82,
 0,
 83,
 0,
 84,
 0,
 85,
 0,
 86,
 0,
 87,
 0,
 88,
 0,
 89,
 0,
 90,
 0,
 91,
 0,
 92,
 0,
 93,
 0,
 94,
 0,
 95,
 0,
 96,
 0,
 97,
 0,
 98,
 0,
 99,
 0,
 100,
 0,
 101,
 0,
 102,
 0,
 103,
 0,
 104,
 0,
 105,
 0,
 106,
 0,
 107,
 0,
 108,
 0,
 109,
 0,
 110,
 0,
 111,
 0,
 112,
 0,
 113,
 0,
 114,
 0,
 115,
 0,
 116,
 0,
 117,
 0,
 118,
 0,
 119,
 0,
 120,
 0,
 121,
 0,
 122,
 0,
 123,
 0,
 124,
 0,
 125,
 0,
 126,
 0,
 127,
 0,
 128,
 0,
 129,
 0,
 130,
 0,
 131,
 0,
 132,
 0,
 133,
 0,
 134,
 0,
 135,
 0,
 136,
 0,
 137,
 0,
 138,
 0,
 139,
 0,
 140,
 0,
 141,
 0,
 142,
 0,
 143,
 0,
 144,
 0,
 145,
 0,
 146,
 0,
 147,
 0,
 148,
 0,
 149,
 0,
 150,
 0,
 151,
 0,
 152,
 0,
 153,
 0,
 154,
 0,
 155,
 0,
 156,
 0,
 157,
 0,
 158,
 0,
 159,
 0,
 160,
 0,
 161,
 0,
 162,
 0,
 163,
 0,
 164,
 0,
 165,
 0,
 166,
 0,
 167,
 0,
 168,
 0,
 169,
 0,
 170,
 0,
 171,
 0,
 172,
 0,
 173,
 0,
 174,
 0,
 175,
 0,
 176,
 0,
 177,
 0,
 178,
 0,
 179,
 0,
 180,
 0,
 181,
 0,
 182,
 0,
 183,
 0,
 184,
 0,
 185,
 0,
 186,
 0,
 187,
 0,
 188,
 0,
 189,
 0,
 190,
 0,
 191,
 0,
 192,
 0,
 193,
 0,
 194,
 0,
 195,
 0,
 196,
 0,
 197,
 0,
 198,
 0,
 199,
 0,
 200,
 0,
 201,
 0,
 202,
 0,
 203,
 0,
 204,
 0,
 205,
 0,
 206,
 0,
 207,
 0,
 208,
 0,
 209,
 0,
 210,
 0,
 211,
 0,
 212,
 0,
 213,
 0,
 214,
 0,
 215,
 0,
 216,
 0,
 217,
 0,
 218,
 0,
 219,
 0,
 220,
 0,
 221,
 0,
 222,
 0,
 223,
 0,
 224,
 0,
 225,
 0,
 226,
 0,
 227,
 0,
 228,
 0,
 229,
 0,
 230,
 0,
 231,
 0,
 232,
 0,
 233,
 0,
 234,
 0,
 235,
 0,
 236,
 0,
 237,
 0,
 238,
 0,
 239,
 0,
 240,
 0,
 241,
 0,
 242,
 0,
 243,
 0,
 244,
 0,
 245,
 0,
 246,
 0,
 247,
 0,
 248,
 0,
 249,
 0,
 250,
 0,
 251,
 0,
 252,
 0,
 253,
 0,
 254,
 0,
 255,
 128,
 0,
 128,
 1,
 128,
 2,
 128,
 3,
 128,
 4,
 128,
 5,
 128,
 6,
 128,
 7,
 128,
 8,
 128,
 9,
 128,
 10,
 128,
 11,
 128,
 12,
 128,
 13,
 128,
 14,
 128,
 15,
 128,
 16,
 128,
 17,
 128,
 18,
 128,
 19,
 128,
 20,
 128,
 21,
 128,
 22,
 128,
 23,
 128,
 24,
 128,
 25,
 128,
 26,
 128,
 27,
 128,
 28,
 128,
 29,
 128,
 30,
 128,
 31,
 128,
 32,
 128,
 33,
 128,
 34,
 128,
 35,
 128,
 36,
 128,
 37,
 128,
 38,
 128,
 39,
 128,
 40,
 128,
 41,
 128,
 42,
 128,
 43,
 128,
 44,
 128,
 45,
 128,
 46,
 128,
 47,
 128,
 48,
 128,
 49,
 128,
 50,
 128,
 51,
 128,
 52,
 128,
 53,
 128,
 54,
 128,
 55,
 128,
 56,
 128,
 57,
 128,
 58,
 128,
 59,
 128,
 60,
 128,
 61,
 128,
 62,
 128,
 63,
 128,
 64,
 128,
 65,
 128,
 66,
 128,
 67,
 128,
 68,
 128,
 69,
 128,
 70,
 128,
 71,
 128,
 72,
 128,
 73,
 128,
 74,
 128,
 75,
 128,
 76,
 128,
 77,
 128,
 78,
 128,
 79,
 128,
 80,
 128,
 81,
 128,
 82,
 128,
 83,
 128,
 84,
 128,
 85,
 128,
 86,
 128,
 87,
 128,
 88,
 128,
 89,
 128,
 90,
 128,
 91,
 128,
 92,
 128,
 93,
 128,
 94,
 128,
 95,
 128,
 96,
 128,
 97,
 128,
 98,
 128,
 99,
 128,
 100,
 128,
 101,
 128,
 102,
 128,
 103,
 128,
 104,
 128,
 105,
 128,
 106,
 128,
 107,
 128,
 108,
 128,
 109,
 128,
 110,
 128,
 111,
 128,
 112,
 128,
 113,
 128,
 114,
 128,
 115,
 128,
 116,
 128,
 117,
 128,
 118,
 128,
 119,
 128,
 120,
 128,
 121,
 128,
 122,
 128,
 123,
 128,
 124,
 128,
 125,
 128,
 126,
 128,
 127,
 128,
 128,
 128,
 129,
 128,
 130,
 128,
 131,
 128,
 132,
 128,
 133,
 128,
 134,
 128,
 135,
 128,
 136,
 128,
 137,
 128,
 138,
 128,
 139,
 128,
 140,
 128,
 141,
 128,
 142,
 128,
 143,
 128,
 144,
 128,
 145,
 128,
 146,
 128,
 147,
 128,
 148,
 128,
 149,
 128,
 150,
 128,
 151,
 128,
 152,
 128,
 153,
 128,
 154,
 128,
 155,
 128,
 156,
 128,
 157,
 128,
 158,
 128,
 159,
 128,
 160,
 128,
 161,
 128,
 162,
 128,
 163,
 128,
 164,
 128,
 165,
 128,
 166,
 128,
 167,
 128,
 168,
 128,
 169,
 128,
 170,
 128,
 171,
 128,
 172,
 128,
 173,
 128,
 174,
 128,
 175,
 128,
 176,
 128,
 177,
 128,
 178,
 128,
 179,
 128,
 180,
 128,
 181,
 128,
 182,
 128,
 183,
 128,
 184,
 128,
 185,
 128,
 186,
 128,
 187,
 128,
 188,
 128,
 189,
 128,
 190,
 128,
 191,
 128,
 192,
 128,
 193,
 128,
 194,
 128,
 195,
 128,
 196,
 128,
 197,
 128,
 198,
 128,
 199,
 128,
 200,
 128,
 201,
 128,
 202,
 128,
 203,
 128,
 204,
 128,
 205,
 128,
 206,
 128,
 207,
 128,
 208,
 128,
 209,
 128,
 210,
 128,
 211,
 128,
 212,
 128,
 213,
 128,
 214,
 128,
 215,
 128,
 216,
 128,
 217,
 128,
 218,
 128,
 219,
 128,
 220,
 128,
 221,
 128,
 222,
 128,
 223,
 128,
 224,
 128,
 225,
 128,
 226,
 128,
 227,
 128,
 228,
 128,
 229,
 128,
 230,
 128,
 231,
 128,
 232,
 128,
 233,
 128,
 234,
 128,
 235,
 128,
 236,
 128,
 237,
 128,
 238,
 128,
 239,
 128,
 240,
 128,
 241,
 128,
 242,
 128,
 243,
 128,
 244,
 128,
 245,
 128,
 246,
 128,
 247,
 128,
 248,
 128,
 249,
 128,
 250,
 128,
 251,
 128,
 252,
 128,
 253,
 128,
 254,
 128,
 255
};

