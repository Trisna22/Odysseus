import logo from './logo.svg';
import './App.css';
import {BrowserRouter, Routes, Route} from "react-router-dom"
import NavigationBar from './components/NavigationBar';
import HomePage from './pages/HomePage';
import NotFoundPage from './pages/NotFoundPage';

// Bootstrap CSS
import "bootstrap/dist/css/bootstrap.min.css";
// Bootstrap Bundle JS
import "bootstrap/dist/js/bootstrap.bundle.min";
import LoginPage from './pages/LoginPage';
import PayloadsPage from './pages/PayloadsPage';
import JobsPage from "./pages/JobsPage";
import ImlantPage from './pages/ImplantPage';

function App() {

    const isAuth = () => {

      return localStorage.getItem("authToken") && 
          localStorage.getItem("authToken");
    }   
  
    return (
    <div className="App">
      <NavigationBar/>
      <BrowserRouter>
        <Routes>
          {
            isAuth() ? 
            <>
              <Route path="/" element={<HomePage/>}/>
              <Route path="/implant" element={<ImlantPage/>}/> 
              <Route path="/payloads" element={<PayloadsPage/>}/>
              <Route path="/jobs" element={<JobsPage/>}/>
            </>
            :
            <Route path="/" element={<LoginPage/>}/>
          }
          <Route path="*" element={<NotFoundPage/>}/>
        </Routes>
      </BrowserRouter>
    </div>
  );
}

export default App;
