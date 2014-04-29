int  OpenADRClient( void );
void InitOpenADRComm( void );

#define PGE
#define PGE_DELAY_SECONDS   120

#define TIME_SYNC_HOUR  4
#define QUERY_SECONDS   15

#define DRAS_LEN    50
#define URL_LEN     249
#define EVENT_LEN   100

typedef struct{
    UINT8               drasClientID[DRAS_LEN]; ///< DRAS Client ID
    UINT8               drasPassword[DRAS_LEN]; ///< DRAS Password
    UINT16              drasServerPort;         ///< DRAS Server Port
    UINT8               AvgPrice;               ///< Average Price
    UINT8               LowPrice;               ///< Low Price
    UINT8               HighPrice;              ///< High Price
    UINT8               SpecialPrice;           ///< Special Price
    UINT8               OpMode;                 ///< Op Mode

    UINT8               EventStatePage[EVENT_LEN];        ///< Event State Page
    UINT8               EventStateConfirmPage[EVENT_LEN]; ///< Event State Confirm Page

    UINT8               drasURL[URL_LEN];       ///< DRAS URL

    UINT32              magic;                  ///< Magic word to check for valid data.

} CFBUF_EPRI;