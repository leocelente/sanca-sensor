'use strict';
const fs = require('fs').promises;
const path = require('path');
const process = require('process');
const {authenticate} = require('@google-cloud/local-auth');
const {google} = require('googleapis');

const SCOPES = ['https://www.googleapis.com/auth/spreadsheets'];
const TOKEN_PATH = path.join(process.cwd(), 'token.json');
const CREDENTIALS_PATH = path.join(process.cwd(), 'client_secret.json');

/**
 * Reads previously authorized credentials from the save file.
 *
 * @return {Promise<OAuth2Client|null>}
 */
async function loadSavedCredentialsIfExist() {
  try {
    const content = await fs.readFile(TOKEN_PATH);
    const credentials = JSON.parse(content);
    return google.auth.fromJSON(credentials);
  } catch (err) {
    return null;
  }
}

/**
 * Serializes credentials to a file comptible with GoogleAUth.fromJSON.
 *
 * @param {OAuth2Client} client
 * @return {Promise<void>}
 */
async function saveCredentials(client) {
  const content = await fs.readFile(CREDENTIALS_PATH);
  const keys = JSON.parse(content);
  const key = keys.installed || keys.web;
  const payload = JSON.stringify({
    type: 'authorized_user',
    client_id: key.client_id,
    client_secret: key.client_secret,
    refresh_token: client.credentials.refresh_token,
  });
  await fs.writeFile(TOKEN_PATH, payload);
}

/**
 * Load or request or authorization to call APIs.
 *
 */
async function authorize() {
  let client = await loadSavedCredentialsIfExist();
  if (client) {
    return client;
  }
  client = await authenticate({
    scopes: SCOPES,
    keyfilePath: CREDENTIALS_PATH,
  });
  if (client.credentials) {
    await saveCredentials(client);
  }
  return client;
}
function writeData(auth, values) {
  const sheets = google.sheets({ version: 'v4', auth });
  const resource = {
    values: [...values],
  };
  sheets.spreadsheets.values.append(
    {
      spreadsheetId: '1M8avl0KYydk-yB5Ota0gDesbiZGc-QHmh0MC-8w3aDg',
      range: 'Data!A1',
      valueInputOption: 'RAW',
      resource: resource,
    },
    (err, result) => {
      if (err) {
        // Handle error
        console.log(err);
      } else {
      }
    }
  );
}

const express = require('express');
const app = express();
const port = 9090;
app.use(express.json())
app.post('/', async (req, res)  =>  {
  console.log(req.body);
  if (req.body.sensor != "bmp280") {
    res.sendStatus(400);  
    return;
  }
  const  {time, sensor, status, temperature, pressure} = req.body;
  
  const date = new Date(time * 1000); 
  res.sendStatus(200);
  let auth = await authorize();
  
  writeData(auth, [date.toISOString(), Number(time).toString(), sensor, status, Number(temperature).toFixed(2), Number(pressure).toFixed(2)]);


})

app.listen(port, () => {
  console.log(`Running server on port: ${port}`)
})

