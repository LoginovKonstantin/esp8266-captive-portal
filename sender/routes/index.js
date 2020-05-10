const {GoogleSpreadsheet} = require('google-spreadsheet');
const express = require('express');
const moment = require('moment');
const creds = require('../creds.json');
const sheetId = '**********************************';

const router = express.Router();

router.get(`/${sheetId}`, async (req, res, next) => {
	await insertIntoSheet({...req.query});
	res.status(200).send({status: 'ok'});
});

async function insertIntoSheet(params) {
	const doc = new GoogleSpreadsheet(sheetId);
	await doc.useServiceAccountAuth(creds);
	await doc.loadInfo();
	const sheet = doc.sheetsByIndex[0];
	await sheet.addRow({...params, date: moment().format()});
}

module.exports = router;
