from fastapi import FastAPI, APIRouter
from fastapi.requests import Request
from fastapi.responses import HTMLResponse, JSONResponse

from app.database import SessionDep

from sqlmodel import select

from app.models.session import *
from app.models.reference import *
from app.models.derived import *


reference_router = APIRouter()
