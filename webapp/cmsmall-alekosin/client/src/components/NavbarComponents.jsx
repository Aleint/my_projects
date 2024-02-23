import React, {useState} from "react";
import {Button, Container, Form, Navbar} from "react-bootstrap";
import {Link, useNavigate} from "react-router-dom";
import {BiUserCircle} from "react-icons/bi";
import Popup from "reactjs-popup";
import "reactjs-popup/dist/index.css";
import {GiConfirmed} from "react-icons/gi";
import {FaHive} from "react-icons/fa";

const buttonStyle = {
    backgroundColor: "transparent",
    color: "white",
    border: "none",
    padding: "0px",
    borderRadius: "0px",
    cursor: "pointer",
};

function NavHeader(props) {
    const navigate = useNavigate();
    const name = props.user && props.user.name;
    const admin = props.user && props.user.administrator;
    const [showB,setShowB]= useState(false);

    const handleTitleChange = (event) => {
        setShowB(true);
        props.setTitle({...props.title, titleName: event.target.value});
    };

    const handleSubmitTitle = (event) => {
        setShowB(false);
        event.preventDefault();
        props.editTitle(props.title);
    };

    return (
        <Navbar bg="primary" variant="dark">
            < Container fluid>
            <FaHive className="fs-2"/>
                {admin ? (
                    <Form onSubmit={handleSubmitTitle} style={{ paddingRight:"0px",marginRight:'0px' }}>
                        <input
                            type="text"
                            value={props.title.titleName}
                            onChange={handleTitleChange}
                            style={{
                                backgroundColor: "transparent",
                                color: "white",
                                border: "none",
                                outline: "none",
                                fontSize: "24px",
                                fontWeight: "bold",
                                padding:"1px",
                            }}
                        />{showB?<Button
                        className="mx-2"
                        style={buttonStyle}
                        type="submit"
                    >
                        <GiConfirmed style={{fontSize: "30px", color: "lightgreen"}}/>
                    </Button>:null}

                    </Form>
                ) : (
                    <span
                        style={{
                            fontSize: "24px",
                            fontWeight: "bold",
                            marginRight: "10px",
                            color: "white",
                        }}
                    >
                        {props.title.titleName}
                    </span>
                )}
                <Navbar.Collapse className="justify-content-end">
                    {name ? (
                        <Popup
                            trigger={
                                <button style={buttonStyle}>
                                    <BiUserCircle style={{fontSize: "36px"}}/>
                                </button>
                            }
                            position="left center"
                        >
                            <>
                                <Navbar>
                                    <div style={{display: "flex", alignItems: "center"}}>
                                        <h6 className="fs-5" style={{marginRight: "1px"}}>
                                            {name}
                                        </h6>
                                        {admin ? <span style={{color: "gray"}}>(admin)</span> : null}
                                        <Link to={"/"}>
                                            <Button
                                                className="mx-1"
                                                variant="danger"
                                                style={{marginLeft: "auto"}}
                                                onClick={props.logout}
                                            >
                                                Logout
                                            </Button>
                                        </Link>
                                    </div>
                                </Navbar>
                            </>
                        </Popup>
                    ) : (
                        <Button
                            className="mx-2"
                            variant="warning"
                            onClick={() => navigate("/login")}
                        >
                            Login
                        </Button>
                    )}
                </Navbar.Collapse>
            </Container>
        </Navbar>
    );
}


export default NavHeader;
