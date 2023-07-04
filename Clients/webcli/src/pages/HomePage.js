import { useEffect, useState } from "react";
import { Button, ButtonGroup, Container, Form, Modal, Spinner, Table, Toast } from "react-bootstrap";
import UserService from "../services/UserService";

const HomePage = () => {

    const userService = new UserService();
    const [slaves, setSlaves] = useState([]);
    const [loading, setLoading] = useState(true);
    const [modalShow, setModalShow] = useState(false);

    const [infoSlave, setInfoSlave] = useState({});
    
    useEffect(() => {
        setInterval(() => {
            userService.getSlaves().then((res) => {
    
                setSlaves(res.data);
                setLoading(false);
    
            }).catch((err) => {
                console.error(err);
                console.error("Failed to retrieve slaves!");
            })
        }, 1000);
    }, [])

    const showModal = (index) => {

        const slave = slaves[index];
        setInfoSlave({
            index: index,
            computername: slave.computername,
            os: slave.os,
            nickname: slave.nickname,
            username: slave.username,
            ip: slave.ip,
            info: slave.info
        })
        setNickName("");
        setModalShow(true);
    }

    const [nickname, setNickName] = useState("");
    const changeNickname = (index) => {
        userService.setNickName(slaves[index].id, nickname).catch((err) => {
            alert("Failed to set nickname! HTTP code: " + err.response.status);
        })
    }
    return (
        <>
        <Modal show={modalShow} onHide={() => setModalShow(false)} size="lg">
            <Modal.Header closeButton>
                <Modal.Title>Implant details</Modal.Title>
            </Modal.Header>
            <Modal.Body>
                <h4>Slave:</h4>
                <Table size="sm">
                <tbody>
                    <tr>
                        <th>Computername</th>
                        <td>{infoSlave.computername}</td>
                    </tr>
                    <tr>
                        <th>Operating system</th>
                        <td>{infoSlave.os}</td>
                    </tr>
                    <tr>
                        <th>Nickname</th>
                        <td>
                            <ButtonGroup>
                                <Form.Control placeholder="Putin's computer" onChange={(e) => setNickName(e.target.value)} value={nickname}></Form.Control>
                                <Button onClick={() => changeNickname(infoSlave.index)} variant="success">Update</Button>
                            </ButtonGroup>
                        </td>
                    </tr>
                    <tr>
                        <th>Username</th>
                        <td>{infoSlave.username}</td>
                    </tr>
                    <tr>
                        <th>IP address</th>
                        <td>{infoSlave.ip}</td>
                    </tr>
                    <tr>
                        <th>Info computer</th>
                        <td>{infoSlave.info}</td>
                    </tr>
                </tbody>
                </Table>
            </Modal.Body>
        </Modal>

        <h1>All implants (Slaves)</h1>
        <Table striped bordered hover>
            <thead>
                <th>Nickname</th>
                <th>IP address</th>
                <th>Username</th>
                <th>Computername</th>
                <th>OS</th>
            </thead>
            <tbody style={{cursor: "pointer"}}>
                {
                    slaves && slaves.length > 0 ? slaves.map((slave, index) => (
                        <tr onClick={() => showModal(index)}>
                            <td>{slave.nickname}</td>
                            <td>{slave.ip}</td>
                            <td>{slave.username}</td>
                            <td>{slave.computername}</td>
                            <td>{slave.info}</td>
                        </tr>
                    )) : <></>
                }
            </tbody>
        </Table>
        {
            loading ? <p>Loading... <Spinner/></p> : <></>
        }
        </>
    )
}

export default HomePage;