package com.mingspy.utils;



import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.Mongo;
import com.mongodb.MongoURI;

public class MongoDriver {

	private static final Mongo.Holder _mongos = new Mongo.Holder();
	private static Log log = LogFactory.getLog(MongoDriver.class);
	
	/**
	 * 
	 * @param strUri URI string likes: mongodb://10.13.95.60/test.answerPatternData
	 * @return
	 */
	public static DBCollection getCollection( String strUri ){
		MongoURI uri = new MongoURI(strUri);
        try {
            Mongo mongo = _mongos.connect( uri );
            DB myDb = mongo.getDB(uri.getDatabase());

            //if there's a username and password
            if(uri.getUsername() != null && uri.getPassword() != null && !myDb.isAuthenticated()) {
                boolean auth = myDb.authenticate(uri.getUsername(), uri.getPassword());
                if(auth) {
                    log.info("Sucessfully authenticated with collection.");
                }
                else {
                    throw new IllegalArgumentException( "Unable to connect to collection." );
                }
            }
            return uri.connectCollection(mongo);
        }
        catch ( final Exception e ) {
            throw new IllegalArgumentException( "Unable to connect to collection." + e.getMessage(), e );
        }
    }
}
