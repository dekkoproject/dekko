CREATE TABLE mailmessages ( 
    id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    type INTEGER NOT NULL,
    parentfolderid INTEGER NOT NULL,
    previousparentfolderid INTEGER NOT NULL,
    sender VARCHAR,
    recipients VARCHAR,
    subject VARCHAR,
    stamp TIMESTAMP,
    status INTEGER,
    parentaccountid INTEGER,
    frommailbox VARCHAR,
    mailfile VARCHAR,
    serveruid VARCHAR,
    size INTEGER,
    contenttype INTEGER,
    responseid INTEGER,
    responsetype INTEGER,
    receivedstamp TIMESTAMP,
    copyserveruid VARCHAR NOT NULL DEFAULT '',
    restorefolderid INTEGER NOT NULL DEFAULT 0,
    listid VARCHAR NOT NULL DEFAULT '',
    rfcid VARCHAR NOT NULL DEFAULT '',
    preview VARCHAR NOT NULL DEFAULT '',
    parentthreadid INTEGER NOT NULL,
    FOREIGN KEY (parentfolderid) REFERENCES mailfolders(id),
    FOREIGN KEY (parentaccountid) REFERENCES mailaccounts(id),
    FOREIGN KEY (parentthreadid) REFERENCES mailthreads(id)
    );

CREATE INDEX parentfolderid_idx ON mailmessages("parentfolderid"); 
CREATE INDEX parentaccountid_idx ON mailmessages("parentaccountid");
CREATE INDEX frommailbox_idx ON mailmessages("frommailbox");
CREATE INDEX stamp_idx ON mailmessages("stamp");
