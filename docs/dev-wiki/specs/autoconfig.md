# Autoconfiguration in Dekko

During the account creation via the setup wizard. Dekko attempts to find an xml or json configuration file for the server
details based on the users domain name.

## XML Configuration file format

This has been modelled on mozilla's ISPDB with a few new additions

```json

<?xml version="1.0"?>
<clientConfig version="1.2">
    <emailProvider id="example.com">
      <domain>example.com</domain>
      <domain>example.net</domain>

      <displayName>Google Mail</displayName>
      <displayShortName>GMail</displayShortName>

      <incomingServer type="imap">
         <hostname>imap.example.com</hostname>
         <port>993</port>
         <socketType>SSL</socketType>
           <!-- "plain": no encryption
                "SSL": SSL 3 or TLS 1 on SSL-specific port
                "STARTTLS": on normal plain port and mandatory upgrade to TLS via STARTTLS
                -->
         <username>%EMAILLOCALPART%</username>
            <!-- "password-cleartext",
                 "plain" (deprecated):
                          Send password in the clear
                          (dangerous, if SSL isn't used either).
                          AUTH PLAIN, LOGIN or protocol-native login.
                 "password-encrypted",
                 "secure" (deprecated):
                           A secure encrypted password mechanism.
                           Can be CRAM-MD5 or DIGEST-MD5. Not NTLM.
                 "NTLM":
                           Use NTLM (or NTLMv2 or successors),
                           the Windows login mechanism.
                 "GSSAPI":
                           Use Kerberos / GSSAPI,
                           a single-signon mechanism used for big sites.
                 "client-IP-address":
                           The server recognizes this user based on the IP address.
                           No authentication needed, the server will require no username nor password.
                 "none":
                           No authentication
         <authentication>password-cleartext</authentication>

         <!-- These are imap related configurations useful for a client to know -->
         <imap>
            <!-- You can explicitly blacklist certain imap extensions if they are known to cause issue.
                 If you are the service provider then you should disable these extensions on the server.
                 For some though it's not possible to access the server in this way, so this provides a means
                 too stop the client from using them. -->
            <blacklist>ID</blacklist>
            <blacklist>CONDSTORE</blacklist>
            <!-- For servers that don't support IDLE you can configure the interval for sending NOOP to keep the connection alive.
                 This interval can vary for various reasons, so this is a way to configure what's best for your server.
                 Note: The interval should be defined in minutes and anything less than 1 minute will get rounded up to 1 minute. -->
            <noopInterval>5</noopInterval>
            <!-- For imap servers that don't support the SPECIAL-USE extension you can set the special folders manually here -->
            <folders>
                <!-- Add full mailbox path including seperator (if any) https://tools.ietf.org/html/rfc6154 -->
                <all>[Gmail]/All Mail</all>
                <archive>[Gmail]/All Mail</archive>
                <drafts>[Gmail]/Drafts</drafts>
                <flagged>[Gmail]/Flagged</flagged>
                <junk>[Gmail]/Spam</junk>
                <sent>[Gmail]/Sent</sent>
                <trash>[Gmail]/Trash</trash>
             </folders>
         </imap>

         <password>optional: the user's password</password>
      </incomingServer>

      <outgoingServer type="smtp">
         <hostname>smtp.googlemail.com</hostname>
         <port>587</port>
         <socketType>STARTTLS</socketType> <!-- see above -->
         <username>%EMAILLOCALPART%</username> <!-- if smtp-auth -->
         <authentication>password-cleartext</authentication>
      </outgoingServer>

      <identities>
         <identity type="primary">
            <name>Dan Chapman</name>
            <email>dan@workemail.com</email>
            <replyTo>dan@otherworkemail.com</replyTo>
            <organization>Dekko Project</organization>
            <signature>Sent using Dekko from my Ubuntu device</signature>
         </identity>
         <identity>
            <name>Dan Chapman</name>
            <email>dan@personalemail.com</email>
            <organization></organization>
            <signature>Sent with love :-)</signature>
         </identity>
      </identities>

      <!-- A page where the ISP describes the configuration.
           This is purely informational and currently mainly for
           maintenance of the files and not used by the client at all.
           Note that we do not necessarily use exactly the config suggested
           by the ISP, e.g. when they don't recommend SSL, but it's available,
           we will configure SSL.
           The text content should contains a description in the native
           language of the ISP (customers), and a short English description,
           mostly for us.
      -->
      <documentation url="http://www.example.com/help/mail/thunderbird">
         <descr lang="en">Configure Dekko for IMAP</descr>
         <descr lang="de">Dekko mit IMAP konfigurieren</descr>
      </documentation>

    </emailProvider>
    <clientConfigUpdate url="https://www.example.com/config/mozilla.xml" />
</clientConfig>

```

## JSON Configuration format

### (Not implemented yet in Dekko v0.6.11, still WIP)

Attribute values are the same as for the xml config so I won't duplicate those here

```json
{
    "version": 1.0,
    "clientConfig": {
        "provider": {
            "domains": {
                "domain": "example.com",
                "domain": "example.org"
             }
         },
         "displayName": "Google Mail",
         "displayShortName": "GMail",
         "servers": {
            "incoming": {
                "imap": {
                    "hostname": "imap.example.org",
                    "port": 993,
                    "socketType": "SSL",
                    "username": "%EMAILLOCALPART%",
                    "authentication": "password-cleartext",
                    "config": {
                        "blacklist": {
                            "extension": "ID",
                            "extension": "CONDSTORE"
                        },
                        "noopInterval": 5,
                        "folders": {
                            "all": "[Gmail]/All Mail",
                            "archive": "[Gmail]/All Mail",
                            "drafts": "[Gmail]/Drafts",
                            "flagged": "[Gmail]/Flagged",
                            "junk": "[Gmail]/Spam",
                            "sent": "[Gmail]/Sent",
                            "trash": "[Gmail]/Trash"
                        }
                    }
                }
            },
            "outgoing": {
                "smtp": {
                    "hostname": "smtp.example.org",
                    "port": 587,
                    "socketType": "STARTTLS",
                    "username": "%EMAILLOCALPART%",
                    "authentication": "GSSAPI"
                }
            }
         },
         "identities": {
            "identity": {
                "primary": true,
                "name": "Dan Chapman",
                "email": "dan@workemail.com",
                "replyTo": "dan@otherworkemail.com",
                "organization": "Dekko Project",
                "signature": "Sent using Dekko from my Ubuntu device"
            },
            "identity": {
                "primary": false,
                "name": "Dan Chapman",
                "email": "dan@personalemail.com",
                "organization": "",
                "signature": "Sent with love :-)"
            }
         },
         "documentation": {}
    }
}

```

## Multiple servers

incomingServer and outgoingServer may appear several times. They are in order of priority,
the first entry should generally be used, unless there's a specific factor or policy which prefers a later config.

For example, there may be configs with STARTTLS and normal SSL. Use the first entry listed.
If that fails (server config change), you can try the second config.

## Placeholders

The email address (before @ or with domain) that the user entered can be used as placeholder in the config file,
so the file is the same for all users (i.e. static).

Placeholders:

* %EMAILADDRESS% (full email address of the user, usually entered by the user)
* %EMAILLOCALPART% (email address, part before @)
* %EMAILDOMAIN% (email address, part after @)
* %REALNAME% (needed?)

These placeholders can be used as value or value part in most settings which take a string.

## Location

For Dekko to be able to find the configuration file it needs to found in a well known location or directly from Dekko's
ISPDB (Only really suitable for large providers, but open to hosting it for you)

If you would just like to use your current conifugration for thunderbird then make sure that it's located at

"http://autoconfig.example.org/mail/config-v1.1.xml" or "http://example.org/.well-known/autoconfig/mail/config-v1.1.xml"

To use an updated new configuration file with the new additional options then make sure it can be located at on of these locations:

### For XML configs

"http://autoconfig.example.org/mail/config-v1.2.xml" or "http://example.org/.well-known/autoconfig/mail/config-v1.2.xml"

### For JSON configs

"http://autoconfig.example.org/mail/config-v1.json" or "http://example.org/.well-known/autoconfig/mail/config-v1.json"

### Dekko ISPDB

If you would like to submit you config to [Dekko's ISPDB](https://autoconfig.dekkoproject.org/v1.2) then submit
a merge proposal or attach the config to a bug report at [launchpad.net/dekko-autoconfig](https://launchpad.net/dekko-autoconfig)

## SRV records

Dekko also supports looking up SRV service labels to configure an IMAP/SMTP server. Any labels added using the methods
outlined in [RFC6186](https://tools.ietf.org/html/rfc6186) will be checked for **after** all checks for autoconfiguration have been done and had no success.

This is pretty much the last chance saloon to find some useful credentials.
