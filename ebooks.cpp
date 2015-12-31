#include <cstdio>
#include <list>
#include <map>
#include "kgramstats.h"
#include <ctime>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <twitcurl.h>
#include <unistd.h>
#include <yaml-cpp/yaml.h>
#include "freevars.h"

int main(int argc, char** args)
{
	srand(time(NULL));
	
	YAML::Node config = YAML::LoadFile("config.yml");
    int delay = config["delay"].as<int>();

	std::ifstream infile(config["corpus"].as<std::string>().c_str());
	std::string corpus;
	std::string line;
	while (getline(infile, line))
	{
		corpus += line + "\n ";
	}

    std::cout << "Preprocessing corpus..." << std::endl;
	kgramstats* stats = new kgramstats(corpus, 4);
    
    std::cout << "Preprocessing freevars..." << std::endl;
    freevars* vars = new freevars();
    vars->addVar("name", "names.txt");
    vars->addVar("noun", "nouns.txt");

    std::cout << "Generating..." << std::endl;
	for (;;)
	{
		std::vector<std::string> doc = stats->randomSentence(rand() % 45 + 5);
		std::string hi;
		for (std::vector<std::string>::iterator it = doc.begin(); it != doc.end(); ++it)
		{
			hi += vars->parse(*it) + " ";
		}

    size_t firstperiod = hi.find_first_of(".!?");
    if (firstperiod != std::string::npos)
    {
      hi = hi.substr(firstperiod+2);
    }
    
    hi.resize(140);

		size_t lastperiod = hi.find_last_of(".!?");
		if ((lastperiod != std::string::npos) && (rand() % 3 > 0))
		{
			hi = hi.substr(0, lastperiod+1);
		}

	    twitCurl twitterObj;
	    std::string tmpStr, tmpStr2;
	    std::string replyMsg;
	    char tmpBuf[1024];
		std::string username(config["username"].as<std::string>());
		std::string password(config["password"].as<std::string>());

	    /* Set twitter username and password */
	    twitterObj.setTwitterUsername(username);
	    twitterObj.setTwitterPassword(password);
	
	    /* OAuth flow begins */
	    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
	    twitterObj.getOAuth().setConsumerKey( config["consumer_key"].as<std::string>() );
	    twitterObj.getOAuth().setConsumerSecret( config["consumer_secret"].as<std::string>() );

	    /* Step 1: Check if we alredy have OAuth access token from a previous run */
	    std::string myOAuthAccessTokenKey("");
	    std::string myOAuthAccessTokenSecret("");
	    std::ifstream oAuthTokenKeyIn;
	    std::ifstream oAuthTokenSecretIn;

	    oAuthTokenKeyIn.open( "twitterClient_token_key.txt" );
	    oAuthTokenSecretIn.open( "twitterClient_token_secret.txt" );

	    memset( tmpBuf, 0, 1024 );
	    oAuthTokenKeyIn >> tmpBuf;
	    myOAuthAccessTokenKey = tmpBuf;

	    memset( tmpBuf, 0, 1024 );
	    oAuthTokenSecretIn >> tmpBuf;
	    myOAuthAccessTokenSecret = tmpBuf;

	    oAuthTokenKeyIn.close();
	    oAuthTokenSecretIn.close();

	    if( myOAuthAccessTokenKey.size() && myOAuthAccessTokenSecret.size() )
	    {
	        /* If we already have these keys, then no need to go through auth again */
	        printf( "\nUsing:\nKey: %s\nSecret: %s\n\n", myOAuthAccessTokenKey.c_str(), myOAuthAccessTokenSecret.c_str() );

	        twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
	        twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );
	    }
	    else
	    {
	        /* Step 2: Get request token key and secret */
	        std::string authUrl;
	        twitterObj.oAuthRequestToken( authUrl );

	        /* Step 3: Get PIN  */
	        memset( tmpBuf, 0, 1024 );
	        printf( "\nDo you want to visit twitter.com for PIN (0 for no; 1 for yes): " );
	        gets( tmpBuf );
	        tmpStr = tmpBuf;
	        if( std::string::npos != tmpStr.find( "1" ) )
	        {
	            /* Ask user to visit twitter.com auth page and get PIN */
	            memset( tmpBuf, 0, 1024 );
	            printf( "\nPlease visit this link in web browser and authorize this application:\n%s", authUrl.c_str() );
	            printf( "\nEnter the PIN provided by twitter: " );
	            gets( tmpBuf );
	            tmpStr = tmpBuf;
	            twitterObj.getOAuth().setOAuthPin( tmpStr );
	        }
	        else
	        {
	            /* Else, pass auth url to twitCurl and get it via twitCurl PIN handling */
	            twitterObj.oAuthHandlePIN( authUrl );
	        }

	        /* Step 4: Exchange request token with access token */
	        twitterObj.oAuthAccessToken();

	        /* Step 5: Now, save this access token key and secret for future use without PIN */
	        twitterObj.getOAuth().getOAuthTokenKey( myOAuthAccessTokenKey );
	        twitterObj.getOAuth().getOAuthTokenSecret( myOAuthAccessTokenSecret );

	        /* Step 6: Save these keys in a file or wherever */
	        std::ofstream oAuthTokenKeyOut;
	        std::ofstream oAuthTokenSecretOut;

	        oAuthTokenKeyOut.open( "twitterClient_token_key.txt" );
	        oAuthTokenSecretOut.open( "twitterClient_token_secret.txt" );

	        oAuthTokenKeyOut.clear();
	        oAuthTokenSecretOut.clear();

	        oAuthTokenKeyOut << myOAuthAccessTokenKey.c_str();
	        oAuthTokenSecretOut << myOAuthAccessTokenSecret.c_str();

	        oAuthTokenKeyOut.close();
	        oAuthTokenSecretOut.close();
	    }
	    /* OAuth flow ends */

	    /* Account credentials verification */
	    if( twitterObj.accountVerifyCredGet() )
	    {
	        twitterObj.getLastWebResponse( replyMsg );
	        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet web response:\n%s\n", replyMsg.c_str() );
	    }
	    else
	    {
	        twitterObj.getLastCurlError( replyMsg );
	        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet error:\n%s\n", replyMsg.c_str() );
	    }
	
	    /* Post a new status message */
	    replyMsg = "";
	    if( twitterObj.statusUpdate( hi ) )
	    {
	        twitterObj.getLastWebResponse( replyMsg );
	        printf( "\ntwitterClient:: twitCurl::statusUpdate web response:\n%s\n", replyMsg.c_str() );
	    }
	    else
	    {
	        twitterObj.getLastCurlError( replyMsg );
	        printf( "\ntwitterClient:: twitCurl::statusUpdate error:\n%s\n", replyMsg.c_str() );
	    }

		sleep(rand() % delay);
	}
	
	return 0;
}
