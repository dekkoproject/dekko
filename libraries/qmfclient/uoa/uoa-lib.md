# Ubuntu online accounts integration

This directory contains the classes for integration with online accounts for
both client and server.

It uses the accounts and signon db's for storing of account username/passwords
as well as generating the required authentication data for the different
messaging services.

There are 2 basic plugins to begin with: Password and OAuthBearer, which you
will find under qmf/plugins/uoa/*

UOA password is the simple password store and retrieve and generates authentication
data using the signon-plugin-sasl library.

OAuthBearer fetches OAuth2 and OAuth1.0a tokens and generates authentication
data for the OAUTHBEARER, OAUTH1.0A and XOAUTH2 sasl mechanisms
