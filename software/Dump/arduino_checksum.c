// src: http://timzaman.wordpress.com/code-c-arduino/checksum-xor-cpp/
// Validates the checksum on an (for instance NMEA) string
// Returns 1 on valid checksum, 0 otherwise
int validateChecksum(void) {
char gotSum[2];
gotSum[0] = buffer[strlen(buffer) - 2];
gotSum[1] = buffer[strlen(buffer) - 1];
// Check that the checksums match up
if ((16 * atoh(gotSum[0])) + atoh(gotSum[1]) == getCheckSum(buffer)) return 1;
else return 0;
}


// Calculates the checksum for a given string
// returns as integer
int getCheckSum(char *string) {
int i;
int XOR;
int c;
// Calculate checksum ignoring any $'s in the string
for (XOR = 0, i = 0; i < strlen(string); i++) {
c = (unsigned char)string[i];
if (c == '*') break;
if (c != '$') XOR ^= c;
}
return XOR;
}