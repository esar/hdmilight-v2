#include <stdio.h>

int main(int argc, char** argv)
{
	if(argc == 3)
	{
		FILE* in = fopen(argv[1], "rb");
		unsigned long maxSize = strtoul(argv[2], NULL, 0);

		if(in)
		{
			int x, y;
			int count = 0;

			while(1)
			{
				if((x = fgetc(in)) == EOF)
					break;
				if((y = fgetc(in)) == EOF)
					y = 0;

				if(count % 8 == 0)
					printf("@%08x ", count * 2);
				printf(" %02x %02x", y, x);
				if(++count % 8 == 0)
					printf("\n");
			}

			if(count % 8 != 0)
				printf("\n");

			if(count * 2 >= maxSize)
			{
				fprintf(stderr, "image is too big: %u vs. %lu\n", count*2, maxSize);
				return 1;
			}
		}
		else
		{
			fprintf(stderr, "failed to open file: %s\n", argv[1]);
			return 1;
		}
	}
	else
	{
		fprintf(stderr, "usage: %s input_file max_size_in_bytes\n", argv[0]);
		return 1;
	}

	return 0;
}
