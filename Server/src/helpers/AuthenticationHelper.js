
// Creates new authentication token.
const newAuthToken = (user) => {

    const token = cryptoHelper.createID();
    databaseHelper.saveAuthToken(token, user);
    return token;
}

// Checks if auth tokens match.
const checkAuthToken = (token) => { 

    return databaseHelper.checkAuthToken(token);
}

module.exports = {
    newAuthToken,
    checkAuthToken
}