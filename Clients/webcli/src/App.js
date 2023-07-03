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

function App() {

  return (
    <div className="App">
      <NavigationBar/>
      <BrowserRouter>
        <Routes>
          {
            localStorage.getItem("authToken") ? 
            <Route path="/" element={<HomePage/>}/> :
            <Route path="/" element={<LoginPage/>}/>
          }
          <Route path="*" element={<NotFoundPage/>}/>
        </Routes>
      </BrowserRouter>
    </div>
  );
}

export default App;
