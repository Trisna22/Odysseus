import axios from "axios"

export default class UserService {

    constructor() {
        const baseURL = localStorage.getItem("baseURL");
        this.instance = axios.create({baseURL: baseURL}); 
    }
    
    connectServer() {
        return this.instance.post("/client/login");
    }

    connectServer(username, password) {
        return this.instance.post("/client/login", {
            username: username,
            password: password
        })
    }


}
