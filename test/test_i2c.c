#include <avr/io.h>

#include <avr/interrupt.h>

#include "../usiTwi/usiTwiSlave.h"

// TWI Slave defines
#define TWI_SLA 0x10  // Slave Address

int main(void)
{

	unsigned char temp;
	static unsigned char i = 0;
	static unsigned char j = 255;
	usiTwiSlaveInit( TWI_SLA );

	sei( );

	for( ; ; )
	{
		if( usiTwiDataInReceiveBuffer() )
		{
			temp = usiTwiReceiveByte();
			usiTwiTransmitByte( i++ );
			usiTwiTransmitByte( temp );
			usiTwiTransmitByte( j-- );
		}

	}
	return 0;

}

