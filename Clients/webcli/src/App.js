import logo from './logo.svg';
import './App.css';
import {BrowserRouter, Routes, Route} from "react-router-dom"
import NavigationBar from './components/NavigationBar';
import HomePage from './pages/SlavesPage';
import NotFoundPage from './pages/NotFoundPage';

// Bootstrap CSS
import "bootstrap/dist/css/bootstrap.min.css";
// Bootstrap Bundle JS
import "bootstrap/dist/js/bootstrap.bundle.min";
import LoginPage from './pages/LoginPage';
import PayloadsPage from './pages/PayloadsPage';
import JobsPage from "./pages/JobsPage";
import ImlantPage from './pages/ImplantPage';
import SlavesPage from './pages/SlavesPage';
import JobManager from './pages/JobManagment';
import ShellPage from './pages/ShellPage';

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
              <Route path="/" element={<ImlantPage/>}/>
              <Route path="/implants" element={<SlavesPage/>}/>
              <Route path="/payloads" element={<PayloadsPage/>}/>
              <Route path="/jobs" element={<JobsPage/>}/>
              <Route path="/job-manager" element={<JobManager/>}></Route>
              <Route path="/shell/:implantId" element={<ShellPage/>}></Route>
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
