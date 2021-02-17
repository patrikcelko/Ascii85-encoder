#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int encode(void)
{
    uint8_t symbolCounter = 0u;
    uint32_t bitSum = 0u;
    uint8_t encodedPart[5] = { 0u };
    int16_t inputChar = 0; // Must be signed, because EOF = -1

    while ((inputChar = getchar()) != EOF || symbolCounter != 0u) {
        if (inputChar == EOF) {
            for (uint8_t addIndex = 1u; addIndex <= 4u - symbolCounter; addIndex++) { // Add missing bits
                bitSum <<= 8u;                                                        // Left shift - 8 bites
                bitSum += '\0';
            }
            symbolCounter = 4u; // Simulate inserting symbols
        } else {
            symbolCounter++;
            bitSum <<= 8u; // Left shift - 8 bites
            bitSum += inputChar;
        }

        if (symbolCounter == 4u) {
            symbolCounter = 0u; // Reset

            for (uint8_t index = 0u; index <= 4u; index++) {
                encodedPart[4u - index] = (bitSum % 85) + 33;
                bitSum /= 85;
            }

            for (uint8_t index = 0u; index <= 4u; index++)
                printf("%c", encodedPart[index]);
        }
    }
    printf("\n");
    return 0;
}

int decode(void)
{
    int16_t inputChar = 0; // Must be signed, because EOF = -1
    uint8_t bytePosition = 5u;
    uint32_t bitSum = 0u;
    uint32_t bitPositionMultiplier[5] = { 1u, 85u, 7225u, 614125u, 52200625u }; // Values of: 85^0, 85^1, 85^2, 85^3, 85^4
    uint8_t decodedPart[4] = { 0u };

    while ((inputChar = getchar()) != EOF) {
        if (isspace(inputChar)) // Skip empty spaces
            continue;

        if (inputChar < 33 || inputChar > 117) // Check if the input value is in range of Ascii85
            return 1;

        bytePosition--;
        bitSum += (inputChar - 33) * bitPositionMultiplier[bytePosition];

        if (bytePosition == 0u) { // We found 5 bytes
            for (uint8_t index = 0u; index <= 3u; index++) {
                decodedPart[3u - index] = bitSum & 0xFFu; // Get 8 lowest bits
                bitSum >>= 8u;                            // Remove 8 lowest bits from number
            }

            for (uint8_t index = 0; index <= 3u; index++)
                printf("%c", decodedPart[index]);

            bitSum = 0u;       // Reset
            bytePosition = 5u; // Reset
        }
    }

    if (bytePosition != 5u) // If we have left overs from decoding, it means that we inserted incorrect Ascii85 value
        return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    int returnCode = 1;

    if (argc == 1 || (argc == 2 && strcmp(argv[1], "-e") == 0)) {
        returnCode = encode();
    } else if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        returnCode = decode();
    } else {
        fprintf(stderr, "usage: %s [-e|-d]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (returnCode != 0) {
        fprintf(stderr, "an error occured\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}