
class DatabaseHelper {

    constructor() {
        // Init database...
        this.tempDB = [];
    }

    saveAuthToken(token, user) {
        this.tempDB.push({
            user: user,
            token: token
        })
    }

    // Retrieves first occurence of auth object with id.
    getAuthTokenById(token) {
        return this.tempDB.filter((entry => token == entry.token))[0];
    }
}

module.exports = DatabaseHelper;